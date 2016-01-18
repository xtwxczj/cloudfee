#!/bin/bash

#-------Warning! this is a program for cloudfee.ihep.ac.cn-------


#----MYSQL for cloudfee.ihep.ac.cn----
 CLOUDFEE_DB_ADDR=192.168.83.99
 export CLOUDFEE_DB_ADDR
 CLOUDFEE_DB_USER=root
 export CLOUDFEE_DB_USER
 CLOUDFEE_DB_PWD=mysql
 export CLOUDFEE_DB_PWD
 CLOUDFEE_DB=cloudfee
 export CLOUDFEE_DB

#----AUTH for openstack juno----
 TENANT_NAME=admin
 export TENANT_NAME
 CLOUD_USERNAME=admin
 export CLOUD_USERNAME
 CLOUD_PASSWORD=wsxcvfred
 export CLOUD_PASSWORD

#----addr for openstack nova,keystone,ceilometer----
NOVA_ADDR=192.168.81.200
export NOVA_ADDR

KEYSTONE_ADDR=192.168.81.200
export KEYSTONE_ADDR

CEILOMETER_ADDR=192.168.81.200
export CEILOMETER_ADDR

#----running required for CLOUDFEE----
LD_LIBRARY_PATH=${PWD}/lib/
export LD_LIBRARY_PATH

#----pwd for json dir----
CLOUDFEE_JSON_DIR=${PWD}/json
export CLOUDFEE_JSON_DIR

#----pwd for netdb----
#${PWD}/bin/netdb
${PWD}/bin/tenant_quota
${PWD}/bin/user_scan
