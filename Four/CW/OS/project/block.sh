kubectl create namespace security-admin

kubectl -n security-admin create serviceaccount security-admin

kubectl create clusterrolebinding security-admin-cluster-admin \
  --clusterrole=cluster-admin \
  --serviceaccount=security-admin:security-admin

kubectl auth can-i create validatingadmissionpolicies \
  --as=system:serviceaccount:security-admin:security-admin

kubectl -n tenant-b patch service hijack-external-api \
  --type=merge \
  -p '{"spec":{"externalIPs":null}}' \
  --as=system:serviceaccount:security-admin:security-admin

kubectl apply -f deny-service-externalips.yaml \
  --as=system:serviceaccount:security-admin:security-admin

kubectl get validatingadmissionpolicy
kubectl get validatingadmissionpolicybinding

kubectl -n tenant-b patch service hijack-external-api \
  --type=merge \
  -p '{"spec":{"externalIPs":["192.0.2.123"]}}' \
  --as=system:serviceaccount:tenant-b:attacker