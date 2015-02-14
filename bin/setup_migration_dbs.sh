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

CONF_FILE=$DIR/../migrations.conf

[ -f $CONF_FILE ] || die "Configuration file does not exist"

. $CONF_FILE

BUILD=$1

MIGRATIONG_NAME=`echo $2 | sed 's/[^a-zA-Z]//g'`

DBSTRING="\$databases['drupal6'] = array("
DBSTRING+="'default' => array("
DBSTRING+="'database' => 'migration_d6_$MIGRATIONG_NAME',"
DBSTRING+="'username' => '$DATABASES_DB_USER',"
DBSTRING+="'password' => '$DRUPAL_SITE_DB_PASS',"
DBSTRING+="'host' => 'localhost',"
DBSTRING+="'port' => '3306',"
DBSTRING+="'driver' => 'mysql',"
DBSTRING+="'prefix' => array("
DBSTRING+="'default'   => '"$MIGRATIONG_NAME"_',"
DBSTRING+="'authmap' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'profile_fields' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'profile_values' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'sequences' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'sessions' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'users' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'vocabulary' => 'migration_d6_mcshared.shared_',"
DBSTRING+="'term_data' => 'migration_d6_mcshared.shared_',"
DBSTRING+="),"
DBSTRING+="),"
DBSTRING+=");"

[ ! -z  $BUILD ] || die "Please provide build derectory path"

[ -d $BUILD ] || die "Build directory $BUILD does not exist"

[ ! `grep -q 'DRUPAL_ROOT' $BUILD/index.php` ] || die "$BUILD does not look like a Drupal installation folder."

chmod 777 $BUILD/sites/default/settings.php

cp $BUILD/sites/default/settings.php $BUILD/sites/default/settings.php.old

echo $DBSTRING >> $BUILD/sites/default/settings.php

chmod 755 $BUILD/sites/default/settings.php

chmod 755 $BUILD/sites/default/settings.php.old

exit 0
