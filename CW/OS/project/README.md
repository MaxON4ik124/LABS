# Flask + Kubernetes Secret lab

Учебная лаборатория: три Flask-микроприложения с формой регистрации/входа.  
Пользователи хранятся в Kubernetes Secret `flask-users-secret` в ключе `users.json`.

## Важно

Это демонстрационная схема. Kubernetes Secret не является базой данных. В реальном проекте используйте БД, внешний Secret Manager и хеширование паролей.

## Запуск

```bash
kind create cluster --config kind-multinode.yaml

helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx
helm repo update
helm install ingress-nginx ingress-nginx/ingress-nginx \
  --namespace ingress-nginx \
  --create-namespace \
  --version 4.14.4

docker build -t flask-secret-auth:local .
kind load docker-image flask-secret-auth:local --name ingress-lab

kubectl apply -f k8s/00-namespace-secret-rbac.yaml
kubectl apply -f k8s/10-flask-apps.yaml
kubectl apply -f k8s/20-ingress.yaml

kubectl get pods -n app -o wide
```

Проверка:

```bash
curl -H "Host: lab.local" http://localhost:8080/app1/
curl -H "Host: lab.local" http://localhost:8080/app2/
curl -H "Host: lab.local" http://localhost:8080/app3/
```

В браузере:

```text
http://localhost:8080/app1/
http://localhost:8080/app2/
http://localhost:8080/app3/
```

## Посмотреть содержимое Secret

```bash
kubectl get secret flask-users-secret -n app \
  -o jsonpath='{.data.users\\.json}' | base64 -d
```

## Удаление

```bash
kind delete cluster --name ingress-lab
```
