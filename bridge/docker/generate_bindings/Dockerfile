FROM python:3

RUN pip install pycparser jinja2

VOLUME [ "/bridge" ]
WORKDIR /bridge

ENTRYPOINT [ "python", "generate_bindings.py" ]