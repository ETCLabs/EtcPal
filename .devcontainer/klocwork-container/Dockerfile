FROM etc-docker.artifactory.etcconnect.com/etc/common-tech/gcc-cmake:llvm13

# Klocwork needs a non-root user, so create one called kwuser
ARG USERNAME=kwuser
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Now set up Klocwork
RUN mkdir -p /klocwork/kwcommandline \
    && mkdir -p /klocwork/artifacts \
    && wget https://klocwork.etcconnect.com/portal/downloads/kw-cmd-installer.linux64.sh -O /klocwork/artifacts/kw-cmd-installer.sh \
    && chown $USERNAME:$USERNAME /klocwork/artifacts/kw-cmd-installer.sh \
    && chmod +x /klocwork/artifacts/kw-cmd-installer.sh \
    && wget https://klocwork.etcconnect.com/portal/downloads/kwbuildtools.24.1.0.77.linux64.zip -O /klocwork/artifacts/kwbuildtools.zip \
    && chown $USERNAME:$USERNAME /klocwork/artifacts/kwbuildtools.zip \
    && wget https://klocwork.etcconnect.com/portal/downloads/p4savscodeextension.vsix -O /klocwork/p4savscodeextension.vsix \
    && chown $USERNAME:$USERNAME /klocwork/p4savscodeextension.vsix \
    && unzip /klocwork/artifacts/kwbuildtools.zip -d /klocwork/ \
    && chown $USERNAME:$USERNAME /klocwork/kwcommandline

# Must be non-root to install and use Klocwork
USER $USERNAME
RUN mkdir -p ~/.klocwork \
    && ./klocwork/artifacts/kw-cmd-installer.sh -a -i /klocwork/kwcommandline \
    && sudo rm -rf /klocwork/artifacts

ENV PATH="${PATH}:/klocwork/kwcommandline/bin:/klocwork/kwbuildtools/bin"
