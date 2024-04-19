FROM etc-docker.artifactory.etcconnect.com/etc/common-tech/clang-cmake-msan:latest
RUN apt-get update && apt-get install -y gdb && \
    python3 -m pip install unity-test-parser