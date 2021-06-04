FROM ubuntu:18.04

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential git tzdata

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN dpkg-reconfigure --frontend noninteractive tzdata

RUN cd /home && git clone https://gitlab+deploy-token-6:S3V9NC8whoqa9xyH-wyD@gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh.git
