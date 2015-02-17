#!/bin/bash

# Don't use me! ... well, do it if you know what you are doing.
#
# In theory I should work; but in practice I'm almost certain that I will
# fail if you run me in a machine that does not looks like the one
# I'm intended to run it. So, don't use me if you are not sure, I can 
# easily get you into trouble.

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

SOURCE="${BASH_SOURCE[0]}"

# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do 
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  # if $SOURCE was a relative symlink, we need to resolve it relative to the path where
  # the symlink file was located
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

while getopts ":hump" opt; do
 case $opt in
  u)
    UPDATE=true
    ;;
  m)
    MAINTENANCES=true
    ;;
  p)
    PREPARE=true
    ;;
  h)
   echo " "
   echo " Usage: ./autobuild.sh"
   echo " "
   echo " Options:"
   echo "   -h           Show brief help"
   echo "   -u           Get the latest make file and do any other task before running jobs."   
   echo "   -m           Run some house cleaning before running job."
   echo "   -p           Takes care of grabbing the most recent production databases and import them."   
   echo " "  
   exit 0
   ;;
  esac
done

# Get the latest make file and do any other task before running jobs
if [ $UPDATE ] ; then $DIR/update.sh ; fi

# Do some house cleaning before running job
if [ $MAINTENANCES ] ; then $DIR/maintenances.sh ; fi

# Takes care of grabbing the most recent production databases and import them
if [ $PREPARE ] ; then $DIR/prepare.sh ; fi

# Build MediaCommons umbrella
# $DIR/build.sh -c configs/mediacommons.conf -m mediacommons.make -s -k
# if [ $? -eq 0 ] ; 
#   then 
#     echo "Successful: Build MediaCommons umbrella" ;
#     # Migrate the content
#     $DIR/migrate.sh -c configs/mediacommons.conf
#   else 
#     die ${LINENO} "build" "Fail: Build MediaCommons umbrella." ; 
# fi ;

# Build Field Guide
$DIR/build.sh -c configs/fieldguide.conf -m mediacommons.make -s -k
if [ $? -eq 0 ] ; 
  then 
    echo "Successful: Build Field Guide" ;
    # Migrate the content
    $DIR/migrate.sh -c configs/fieldguide.conf
  else 
    echo ${LINENO} "build" "Fail: Build Field Guide." ; 
fi ;

exit 0

# Build Alt-Academy
$DIR/build.sh -c configs/alt-ac.conf -m mediacommons.make -s -k
if [ $? -eq 0 ] ; then echo "Successful: Build Alt-Academy" ; else echo ${LINENO} "build" "Fail: Build Alt-Academy." ; fi ;

# Build [in]Transition
$DIR/build.sh -c configs/intransition.conf -m mediacommons.make -s -k
if [ $? -eq 0 ] ; then echo "Successful: Build [in]Transition" ; else echo ${LINENO} "build" "Fail: Build [in]Transition." ; fi ;

# Build In Media Res
$DIR/build.sh -c configs/imr.conf -m mediacommons.make -s -k
if [ $? -eq 0 ] ; then echo "Successful: Build In Media Res" ; else echo ${LINENO} "build" "Fail: Build In Media Res." ; fi ;

# Build The New Everyday
$DIR/build.sh -c configs/tne.conf -m mediacommons.make -s -k
if [ $? -eq 0 ] ; then echo "Successful: Build The New Everyday" ; else echo ${LINENO} "build" "Fail: Build The New Everyday." ; fi ;

# Run a basic test and report if error
# ./bin/report_error.sh

exit 0
