FROM phusion/baseimage:0.11

WORKDIR /home/root

RUN \
   apt-get clean \
   && apt-get update \
   && apt-get install -y \
     autoconf \
     automake \
     cmake \
     g++ \
     git \
     libbz2-dev \
     libsnappy-dev \
     libssl-dev \
     libtool \
     make \
     pkg-config \
     python3 \
     python3-jinja2 \
     ruby \
     ruby-dev \
     scrypt \
     dnsutils \
     libboost-chrono-dev \
     libboost-context-dev \
     libboost-coroutine-dev \
     libboost-date-time-dev \
     libboost-filesystem-dev \
     libboost-iostreams-dev \
     libboost-locale-dev \
     libboost-program-options-dev \
     libboost-serialization-dev \
     libboost-signals-dev \
     libboost-system-dev \
     libboost-test-dev \
     libboost-thread-dev

# NOTE: Add this to instructions
COPY xgt-ruby /home/root/xgt-ruby
COPY . /home/root/xgt

RUN gem update --system && gem install bundler rake
RUN cd xgt-ruby && bundle
RUN \
  cd xgt \
  && rake clean configure make

EXPOSE 8751
EXPOSE 8090
EXPOSE 2001

CMD cd xgt && rake run

# sudo docker run \
#   --interactive \
#   --tty \
#   --publish 8751:8751 \
#   --publish 8090:8090 \
#   --publish 2001:2001 \
#   --env XGT_HOST=http://demo-superproducers-lb-1807013199.us-east-1.elb.amazonaws.com:8751 \
#   --env XGT_NAME=XGT32P19NiqGKhTj \
#   --env XGT_WITNESS_PRIVATE_KEY=5KQ3CekviHBZPPSm7Hh7QrSxueQ13kQbmmUmc4Mf5xbc9LBejyr \
#   --env XGT_RECOVERY_PRIVATE_KEY=5J8rMsuzs5S1cGXyQCbEnfqzoMXpYyyhiJ7evJVDBe7ffNX7h5x \
#   --rm \
#   xgt:0.0.1
