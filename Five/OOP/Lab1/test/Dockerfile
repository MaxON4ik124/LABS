FROM ubuntu:24.04

RUN apt-get update

RUN apt-get install -y -q clang
RUN apt-get install -y -q valgrind
RUN apt-get install -y -q python3-pip
RUN apt-get install -y -q python3-venv

RUN python3 -m venv /venv
RUN /venv/bin/python3 -m pip install pybind11
RUN /venv/bin/python3 -m pip install pytest

RUN useradd -ms /bin/bash testuser

COPY ./test.sh /test/test.sh
COPY ./main.cpp /test/main.cpp
COPY ./tests /test/tests

COPY ./wrapper_test /test/wrapper_test

RUN mkdir /src/
RUN chown -R testuser:testuser /src/

WORKDIR /test/

USER testuser

CMD ["/test/test.sh"]
