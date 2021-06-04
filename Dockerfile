FROM ubuntu:20.04

WORKDIR /home/root

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get clean && apt-get update
RUN apt-get install -y \
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
     scrypt \
     dnsutils \
     ccache
RUN apt-get install -y \
     libboost-all-dev
RUN apt-get install -y \
     build-essential \
     curl \
     zlib1g-dev \
     libreadline-dev \
     libyaml-dev \
     libxml2-dev \
     libxslt-dev

RUN git clone https://github.com/sstephenson/rbenv.git /root/.rbenv
RUN git clone https://github.com/sstephenson/ruby-build.git /root/.rbenv/plugins/ruby-build
RUN /root/.rbenv/plugins/ruby-build/install.sh
ENV PATH /root/.rbenv/bin:$PATH
RUN echo 'eval "$(rbenv init -)"' >> /etc/profile.d/rbenv.sh # or /etc/profile
RUN echo 'eval "$(rbenv init -)"' >> .bashrc
ENV CONFIGURE_OPTS --disable-install-doc
RUN rbenv install 2.7.2
RUN rbenv global 2.7.2
RUN rbenv rehash

RUN ["/bin/bash", "-c", "source /home/root/.bashrc && gem update --system"]
RUN ["/bin/bash", "-c", "source /home/root/.bashrc && gem install --no-document bundler rake xgt-ruby"]
RUN ["/bin/bash", "-c", "source /home/root/.bashrc && rbenv rehash"]

COPY . /home/root/xgt

RUN ["/bin/bash", "-c", "source /home/root/.bashrc && cd xgt && rake clean"]
RUN ["/bin/bash", "-c", "source /home/root/.bashrc && cd xgt && rake configure"]
RUN ["/bin/bash", "-c", "source /home/root/.bashrc && cd xgt && rake make"]

# RUN cd xgt && rake clean
# RUN cd xgt && rake configure
# RUN cd xgt && rake make

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
