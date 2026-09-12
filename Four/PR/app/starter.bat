@echo off
setlocal EnableExtensions

title XXE Demo Cluster Deployment

rem Directory containing this batch file.
set "PROJECT_DIR=%~dp0"
set "PROFILE=xxe-demo"
set "NAMESPACE=xxe-lab"

rem Switch to the project directory before running Docker builds.
cd /d "%PROJECT_DIR%" 

echo ==================================================
echo Checking Docker
echo ==================================================

docker info >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Docker Desktop is unavailable.
    goto :error
)

echo ==================================================
echo Removing old proxy containers and the old cluster
echo ==================================================

for %%C in (xxe-v1-proxy xxe-v2-proxy xxe-v3-proxy) do (
    docker rm -f %%C >nul 2>&1
)

minikube delete -p "%PROFILE%"

echo ==================================================
echo Creating the Minikube cluster
echo ==================================================

minikube start ^
  -p "%PROFILE%" ^
  --driver=docker ^
  --container-runtime=docker ^
  --nodes=4 ^
  --cpus=2 ^
  --memory=2200 ^
  --delete-on-failure

if errorlevel 1 goto :error

kubectl config use-context "%PROFILE%" 
minikube status -p "%PROFILE%" 

echo ==================================================
echo Creating the namespace, storage, and node labels
echo ==================================================

kubectl apply -f "%PROJECT_DIR%namespace.yaml" 
minikube -p "%PROFILE%" addons enable volumesnapshots 
minikube -p "%PROFILE%" addons enable csi-hostpath-driver

kubectl label node "%PROFILE%-m02" app-variant=v1 --overwrite 
kubectl label node "%PROFILE%-m03" app-variant=v2 --overwrite 
kubectl label node "%PROFILE%-m04" app-variant=v3 --overwrite 

kubectl get nodes -L app-variant -o wide

echo ==================================================
echo Deploying the database Secret, Service, and StatefulSet
echo ==================================================

kubectl apply -f "%PROJECT_DIR%db-secret.yaml" 
kubectl apply -f "%PROJECT_DIR%orders-db.yaml" 
kubectl apply -f "%PROJECT_DIR%db.yaml" 

@REM kubectl rollout status statefulset/shop-db -n "%NAMESPACE%" --timeout=300s
@REM if errorlevel 1 goto :database_error

echo ==================================================
echo Building the three application variants
echo ==================================================

rem The current directory is PROJECT_DIR, so use "." as the Docker build context.
rem This avoids problems caused by the trailing backslash in %%~dp0.
docker build -f Dockerfile --build-arg APP_VARIANT=v1 -t xxe-demo:v1 . 
docker build -f Dockerfile --build-arg APP_VARIANT=v2 -t xxe-demo:v2 . 
docker build -f Dockerfile --build-arg APP_VARIANT=v3 -t xxe-demo:v3 . 

minikube -p "%PROFILE%" image load xxe-demo:v1 --overwrite 
minikube -p "%PROFILE%" image load xxe-demo:v2 --overwrite 
minikube -p "%PROFILE%" image load xxe-demo:v3 --overwrite 

echo ==================================================
echo Deploying the applications
echo ==================================================

kubectl apply -f "%PROJECT_DIR%apps.yaml" 

kubectl rollout status deployment/shop-v1 -n "%NAMESPACE%" --timeout=300s || goto :apps_error
kubectl rollout status deployment/shop-v2 -n "%NAMESPACE%" --timeout=300s || goto :apps_error
kubectl rollout status deployment/shop-v3 -n "%NAMESPACE%" --timeout=300s || goto :apps_error

kubectl get deployments,statefulsets,pods,services,pvc -n "%NAMESPACE%" -o wide

echo ==================================================
echo Creating local TCP proxies for ports 5001-5003
echo ==================================================

rem Build the local proxy image from the current project directory.
docker build -f Dockerfile.proxy -t xxe-node-proxy:local . 

for %%C in (xxe-v1-proxy xxe-v2-proxy xxe-v3-proxy) do (
    docker rm -f %%C >nul 2>&1
)

rem Detect the Docker network used by the Minikube nodes.
for /f "usebackq delims=" %%N in (`docker inspect -f "{{range $k,$v := .NetworkSettings.Networks}}{{$k}}{{end}}" "%PROFILE%-m02"`) do set "MINIKUBE_NETWORK=%%N"
if not defined MINIKUBE_NETWORK goto :error

echo Docker network: %MINIKUBE_NETWORK%

rem Start one proxy container for each application version.
docker run -d --name xxe-v1-proxy --restart unless-stopped --network "%MINIKUBE_NETWORK%" -p 5001:5001 -e LISTEN_PORT=5001 -e TARGET_HOST=%PROFILE%-m02 -e TARGET_PORT=5001 xxe-node-proxy:local 
docker run -d --name xxe-v2-proxy --restart unless-stopped --network "%MINIKUBE_NETWORK%" -p 5002:5002 -e LISTEN_PORT=5002 -e TARGET_HOST=%PROFILE%-m03 -e TARGET_PORT=5002 xxe-node-proxy:local 
docker run -d --name xxe-v3-proxy --restart unless-stopped --network "%MINIKUBE_NETWORK%" -p 5003:5003 -e LISTEN_PORT=5003 -e TARGET_HOST=%PROFILE%-m04 -e TARGET_PORT=5003 xxe-node-proxy:local 

echo.
echo ==================================================
echo Cluster deployment completed successfully
echo ==================================================
echo V1: http://127.0.0.1:5001
echo V2: http://127.0.0.1:5002
echo V3: http://127.0.0.1:5003
echo.
echo To run the callback server for the blind scenario, use:
echo python callback_server.py
echo.

exit /b 0

:database_error
echo [ERROR] PostgreSQL did not start.
kubectl get pods -n "%NAMESPACE%" -o wide
kubectl describe pod shop-db-0 -n "%NAMESPACE%"
kubectl logs shop-db-0 -n "%NAMESPACE%"
goto :error

:apps_error
echo [ERROR] One or more applications did not start.
kubectl get pods -n "%NAMESPACE%" -o wide
kubectl get events -n "%NAMESPACE%" --sort-by=.metadata.creationTimestamp
goto :error

:error
echo.
echo ==================================================
echo Deployment failed
echo ==================================================
pause
exit /b 1
