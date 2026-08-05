FROM python:3.12-slim

ARG APP_VARIANT=v3

WORKDIR /app

ENV APP_VARIANT=${APP_VARIANT}
ENV PYTHONUNBUFFERED=1

COPY requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt

COPY app.py .
COPY models.py .
COPY templates ./templates
COPY static ./static
COPY data ./data

EXPOSE 5000

CMD ["python", "app.py"]