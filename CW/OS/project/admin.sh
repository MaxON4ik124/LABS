kubectl create serviceaccount full-user -n app
kubectl delete validatingwebhookconfiguration ingress-nginx-admission
kubectl create clusterrolebinding full-user-admin \
  --clusterrole=cluster-admin \
  --serviceaccount=app:full-user

kubectl auth can-i get secret/flask-users-secret -n app \
  --as=system:serviceaccount:app:full-user

kubectl get secret flask-users-secret -n app \
  --as=system:serviceaccount:app:full-user \
  -o jsonpath='{.data.users\.json}' | base64 -d | python3 -m json.tool

