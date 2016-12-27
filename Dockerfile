FROM ubuntu:trusty

RUN apt-get -y -qq update && \
    apt-get -y -qq install build-essential devscripts equivs

COPY . /build/metrics
RUN cd /build/metrics && \
    DEBIAN_FRONTEND=noninteractive mk-build-deps -ir -t "apt-get -qq --no-install-recommends"
RUN cd /build/metrics && \
    yes | debuild -e CC -e CXX -uc -us -j$(cat /proc/cpuinfo | fgrep -c processor) && \
    debi
