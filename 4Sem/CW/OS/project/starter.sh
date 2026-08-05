eval $(minikube docker-env --unset)

minikube delete -p minikube

minikube start \
  --driver=docker \
  --kubernetes-version=v1.33.1 \
  --nodes=3

kubectl label node minikube-m02 tenant=victim --overwrite
kubectl label node minikube-m03 tenant=attacker --overwrite

kubectl get nodes --show-labels
kubectl get pods -n kube-system
kubectl get pods -n kube-system -l k8s-app=kube-proxy

kubectl create namespace tenant-a
kubectl create namespace tenant-b

kubectl -n tenant-b create serviceaccount attacker

kubectl apply -f attacker-rbac.yaml

kubectl -n tenant-a create secret generic payment-api-token \
  --from-literal=API_TOKEN='SECRET_DATA_FROM_TENANT_A'

kubectl apply -f victim.yaml

kubectl -n tenant-a wait \
  --for=condition=Ready pod/victim \
  --timeout=120s
