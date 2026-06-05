#!/usr/bin/env bash
set -euo pipefail


K8S_VERSION="v1.30.0"
CPUS="4"
MEMORY="4096"

IMAGE_NAME="flask-secret-auth:local"

INGRESS_NAMESPACE="ingress-nginx"
INGRESS_RELEASE="ingress-nginx"

INGRESS_NGINX_CHART_VERSION="4.14.4"

APP_NAMESPACE="app"
HOSTNAME="lab.local"

HTTP_NODEPORT="30080"
HTTPS_NODEPORT="30443"

command -v minikube >/dev/null 2>&1 || {
  echo "ERROR: minikube не найден"
  exit 1
}

command -v kubectl >/dev/null 2>&1 || {
  echo "ERROR: kubectl не найден"
  exit 1
}

command -v helm >/dev/null 2>&1 || {
  echo "ERROR: helm не найден"
  exit 1
}

command -v docker >/dev/null 2>&1 || {
  echo "ERROR: docker не найден"
  exit 1
}

if [[ ! -f Dockerfile ]]; then
  echo "ERROR: Dockerfile не найден. Запускай скрипт из корня проекта flask-k8s-secret-lab."
  exit 1
fi

if [[ ! -d k8s ]]; then
  echo "ERROR: директория k8s не найдена. Запускай скрипт из корня проекта flask-k8s-secret-lab."
  exit 1
fi


echo "[1/10] Удаляю существующие minikube-кластеры..."
minikube delete --all || true

echo "[2/10] Запускаю minikube..."
minikube start \
  --driver=docker \
  --kubernetes-version="${K8S_VERSION}" \
  --cpus="${CPUS}" \
  --memory="${MEMORY}"

echo "[3/10] Переключаю Docker CLI на Docker внутри minikube..."
eval "$(minikube docker-env)"

echo "[4/10] Собираю Docker image приложения..."
docker build -t "${IMAGE_NAME}" .

echo "[5/10] Добавляю Helm repo ingress-nginx..."
helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx >/dev/null 2>&1 || true
helm repo update

echo "[6/10] Устанавливаю ingress-nginx..."
helm upgrade --install "${INGRESS_RELEASE}" ingress-nginx/ingress-nginx \
  --namespace "${INGRESS_NAMESPACE}" \
  --create-namespace \
  --version "${INGRESS_NGINX_CHART_VERSION}" \
  --set controller.service.type=NodePort \
  --set controller.service.nodePorts.http="${HTTP_NODEPORT}" \
  --set controller.service.nodePorts.https="${HTTPS_NODEPORT}" \
  --wait \
  --timeout 5m

echo "[7/10] Создаю namespace приложения..."
kubectl create namespace "${APP_NAMESPACE}" --dry-run=client -o yaml | kubectl apply -f -

echo "[8/10] Применяю Kubernetes manifests..."
kubectl apply -f k8s/00-namespace-secret-rbac.yaml
kubectl apply -f k8s/10-flask-apps.yaml
kubectl apply -f k8s/20-ingress.yaml
kubectl apply -f some-config.yaml

echo "[9/10] Жду готовности ingress-nginx и приложений..."
kubectl rollout status deployment/ingress-nginx-controller -n "${INGRESS_NAMESPACE}" --timeout=180s

kubectl rollout status deployment/flask-app1 -n "${APP_NAMESPACE}" --timeout=180s || true
kubectl rollout status deployment/flask-app2 -n "${APP_NAMESPACE}" --timeout=180s || true
kubectl rollout status deployment/flask-app3 -n "${APP_NAMESPACE}" --timeout=180s || true

echo "[10/10] Проверяю состояние..."
kubectl get pods -n "${INGRESS_NAMESPACE}" -o wide
kubectl get svc -n "${INGRESS_NAMESPACE}"
kubectl get pods -n "${APP_NAMESPACE}" -o wide
kubectl get ingress -n "${APP_NAMESPACE}"

MINIKUBE_IP="$(minikube ip)"

echo
echo "========================================"
echo "Minikube IP: ${MINIKUBE_IP}"
echo "========================================"
echo
echo "Проверь через curl:"
echo
echo "curl -H \"Host: ${HOSTNAME}\" \"http://${MINIKUBE_IP}:${HTTP_NODEPORT}/app1/\""
echo "curl -H \"Host: ${HOSTNAME}\" \"http://${MINIKUBE_IP}:${HTTP_NODEPORT}/app2/\""
echo "curl -H \"Host: ${HOSTNAME}\" \"http://${MINIKUBE_IP}:${HTTP_NODEPORT}/app3/\""
echo
echo "Для браузера добавь ${HOSTNAME} в /etc/hosts:"
echo
echo "Linux:"
echo "  echo \"${MINIKUBE_IP} ${HOSTNAME}\" | sudo tee -a /etc/hosts"
echo
echo "macOS:"
echo "  sudo sed -i '' '/${HOSTNAME}/d' /etc/hosts"
echo "  echo \"${MINIKUBE_IP} ${HOSTNAME}\" | sudo tee -a /etc/hosts"
echo
echo "URL для браузера:"
echo
echo "  http://${HOSTNAME}:${HTTP_NODEPORT}/app1/"
echo "  http://${HOSTNAME}:${HTTP_NODEPORT}/app2/"
echo "  http://${HOSTNAME}:${HTTP_NODEPORT}/app3/"
echo
echo "Если доступ через NodePort не работает на macOS/Windows с Docker driver:"
echo
echo "  kubectl port-forward -n ${INGRESS_NAMESPACE} svc/ingress-nginx-controller 8080:80"
echo
echo "И затем:"
echo
echo "  http://${HOSTNAME}:8080/app1/"
echo "  http://${HOSTNAME}:8080/app2/"
echo "  http://${HOSTNAME}:8080/app3/"
echo