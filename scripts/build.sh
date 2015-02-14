#!/bin/bash

die () {
  echo $1
  exit 1
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

DEBUG=""

while getopts ":c:m:hdsi" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die "Configuration file does not exist." 
   CONF_FILE=$OPTARG
   ;;
  m)
    [ -f $OPTARG ] || die "Make file does not exist."
    MAKE_FILE=$OPTARG
    ;;
  d)
    DEBUG='-d -v'
    ;;
  s)
    SASS=true
    ;;
  i)
    MIGRATE=true
    ;;    
  h)
   echo " "
   echo " Usage: ./build.sh -m example.make -c example.conf"
   echo " "
   echo " Options:"
   echo "   -h           Show brief help"
   echo "   -i           Run migration script after install"
   echo "   -s           Find SASS based themes and compile"   
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -m <file>    Specify the make file to use (e.g., -m example.make)."
   echo " "  
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die "No configuration file provided."

[ $MAKE_FILE ] || die "No make file provided."

# load configuration file
. $CONF_FILE

[ -w $BUILD_DIR ] || die "Unable to write to $BUILD_DIR directory"

[ -d $BUILD_DIR ] || mkdir $BUILD_DIR

if [ -z "$DRUPAL_ACCOUNT_PASS" -a "$DRUPAL_ACCOUNT_PASS"==" " ]; then
  DRUPAL_ACCOUNT_PASS=`drush php-eval 'print MD5(microtime());'`
fi

echo Prepare new site using $MAKE_FILE

# download and prepare for the installation using make file
drush ${DEBUG} make --prepare-install -y ${MAKE_FILE} ${BUILD_DIR}/${BUILD_NAME}  --uri=${BASE_URL}

[ -d $BUILD_DIR/$BUILD_NAME ] || die "Unable to install new site. Build does not exist" 

# reuse code that has been linked in the lib folder
$DIR/link_build.sh $BUILD_DIR/$BUILD_NAME

echo Install new site

# this long string runs the site installation
drush $DEBUG -y site-install $DRUPAL_INSTALL_PROFILE_NAME --site-name="$DRUPAL_SITE_NAME" --account-pass="$DRUPAL_ACCOUNT_PASS" --account-name=$DRUPAL_ACCOUNT_NAME --account-mail=$DRUPAL_ACCOUNT_MAIL --site-mail=$DRUPAL_SITE_MAIL --db-url=$DRUPAL_SITE_DB_TYPE://$DRUPAL_SITE_DB_USER:$DRUPAL_SITE_DB_PASS@$DRUPAL_SITE_DB_ADDRESS/$DRUPAL_DB_NAME --root=$BUILD_DIR/$BUILD_NAME

chmod 777 $BUILD_DIR/$BUILD_NAME/sites/default/settings.php

chmod 777 $BUILD_DIR/$BUILD_NAME/sites/default

# remove text files and rename install.php to install.php.off
$DIR/cleanup.sh $BUILD_DIR/$BUILD_NAME

# find SASS config.rb and compile the CSS file
if [ $SASS ] ; then $DIR/sass.sh $BUILD_DIR/$BUILD_NAME ; fi

# link to the lastes build if BUILD_BASE_NAME it's different from BUILD_NAME
if [ $BUILD_BASE_NAME != $BUILD_NAME ]; then
  # build base name its a link?
  if [[ -h $BUILD_DIR/$BUILD_BASE_NAME ]]; then
    cd $BUILD_DIR
    rm $BUILD_DIR/$BUILD_BASE_NAME
    ln -s $BUILD_NAME $BUILD_BASE_NAME
    cd -
  fi
fi

# instead of having a base them and child theme; mediaccomons theme try to do 
# everything in just one theme, must assing this class 
$DIR/theme_variable.sh ${BUILD_DIR}/${BUILD_NAME} ${DRUPAL_SPECIAL_BODY_CLASS}

echo Build path $BUILD_DIR/$BUILD_NAME

# do a quick status check
$DIR/check_build.sh $BUILD_DIR/$BUILD_NAME $BASE_URL

# share cookie
echo "\$cookie_domain = '${COOKIE_DOMAIN}';" >> ${BUILD_DIR}/${BUILD_NAME}/sites/default/settings.php

if [ $MIGRATE ] ; then $DIR/migrate.sh -c $CONF_FILE $DEBUG ; fi

chmod 755 $BUILD_DIR/$BUILD_NAME/sites/default/settings.php

chmod 755 $BUILD_DIR/$BUILD_NAME/sites/default

exit 0
