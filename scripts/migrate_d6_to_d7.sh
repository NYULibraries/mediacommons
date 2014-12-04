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

[ -f $CONF_FILE ] || die "Configuration file does not exist."

CONF_PERM=`ls -l $CONF_FILE | awk '{print $1}'`

# make this a bit more robust
if [ "$CONF_PERM" != "-rwx------@" ] 
  then 
    echo "Please change configuration file permission to be read only by owner." 
fi

. $CONF_FILE

# For each site we check if the given directory in the configuration file
# looks like a Drupal 7 installation. If TRUE, run the migration script.

# In transition
# check if this directory looks like Drupal 7
MATCH=`grep -c 'DRUPAL_ROOT' $ROOT_DIR_INTRANSITION/index.php` 
if [ $MATCH -gt 0 ]
  then
  
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal

    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL_INTRANSITION --root=$ROOT_DIR_INTRANSITION --user=1`
    if [[ $SITE_ONLINE =~ "Connected" ]]
      then
        if [[ $SITE_ONLINE =~ "Successful" ]]
          then      
            echo $MIGRATION_TASK_INTRANSITION
            drush -d -v scr $MIGRATION_SCRIPT_INTRANSITION --uri=$BASE_URL_INTRANSITION --root=$ROOT_DIR_INTRANSITION --user=1 --task="$MIGRATION_TASK_INTRANSITION"
        fi
    fi
    
  else
    echo Unable to "$MIGRATION_TASK_INTRANSITION" make sure $ROOT_DIR_INTRANSITION its the root directory of Drupal 7 installation.
fi

# The New Everyday
# check if this directory looks like Drupal 7
MATCH=`grep -c 'DRUPAL_ROOT' $ROOT_DIR_TNE/index.php` 
if [ $MATCH -gt 0 ]
  then
  
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal

    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL_TNE --root=$ROOT_DIR_TNE --user=1`
    if [[ $SITE_ONLINE =~ "Connected" ]]
      then
        if [[ $SITE_ONLINE =~ "Successful" ]]
          then      
            echo $MIGRATION_TASK_TNE
            drush -d -v scr $MIGRATION_SCRIPT_TNE --uri=$BASE_URL_TNE --root=$ROOT_DIR_TNE --user=1 --task="$MIGRATION_TASK_TNE"
        fi
    fi

  else
    echo Unable to "$MIGRATION_TASK_TNE" make sure $ROOT_DIR_TNE its the root directory of Drupal 7 installation.
fi

# Alt-Academy
# check if this directory looks like Drupal 7
MATCH=`grep -c 'DRUPAL_ROOT' $ROOT_DIR_ALTAC/index.php` 
if [ $MATCH -gt 0 ]
  then
  
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal

    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL_ALTAC --root=$ROOT_DIR_ALTAC --user=1`
    if [[ $SITE_ONLINE =~ "Connected" ]]
      then
        if [[ $SITE_ONLINE =~ "Successful" ]]
          then      
            echo $MIGRATION_TASK_ALTAC
            drush -d -v scr $MIGRATION_SCRIPT_ALTAC --uri=$BASE_URL_ALTAC --root=$ROOT_DIR_ALTAC --user=1 --task="$MIGRATION_TASK_ALTAC"
        fi
    fi

  else
    echo Unable to "$MIGRATION_TASK_ALTAC" make sure $ROOT_DIR_ALTAC its the root directory of Drupal 7 installation.
fi

# In Media Res
# check if this directory looks like Drupal 7
MATCH=`grep -c 'DRUPAL_ROOT' $ROOT_DIR_IMR/index.php` 
if [ $MATCH -gt 0 ]
  then
  
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal

    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL_IMR --root=$ROOT_DIR_IMR --user=1`
    if [[ $SITE_ONLINE =~ "Connected" ]]
      then
        if [[ $SITE_ONLINE =~ "Successful" ]]
          then      
            echo $MIGRATION_TASK_IMR
            drush -d -v scr $MIGRATION_SCRIPT_IMR --uri=$BASE_URL_IMR --root=$ROOT_DIR_IMR --user=1 --task="$MIGRATION_TASK_IMR"
        fi
    fi

  else
    echo Unable to "$MIGRATION_TASK_IMR" make sure $ROOT_DIR_IMR its the root directory of Drupal 7 installation.
fi

# MediaCommons
# check if this directory looks like Drupal 7
MATCH=`grep -c 'DRUPAL_ROOT' $ROOT_DIR_MEDIACOMMONS/index.php` 
if [ $MATCH -gt 0 ]
  then
  
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal

    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL_MEDIACOMMONS --root=$ROOT_DIR_MEDIACOMMONS --user=1`
    if [[ $SITE_ONLINE =~ "Connected" ]]
      then
        if [[ $SITE_ONLINE =~ "Successful" ]]
          then      
            echo $MIGRATION_TASK_MEDIACOMMONS
            drush -d -v scr $MIGRATION_SCRIPT_MEDIACOMMONS --uri=$BASE_URL_MEDIACOMMONS --root=$ROOT_DIR_MEDIACOMMONS --user=1 --task="$MIGRATION_TASK_MEDIACOMMONS"
        fi
    fi

  else
    echo Unable to "$MIGRATION_TASK_MEDIACOMMONS" make sure $ROOT_DIR_MEDIACOMMONS its the root directory of Drupal 7 installation.
fi
