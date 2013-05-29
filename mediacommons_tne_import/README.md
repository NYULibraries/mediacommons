MediaCommons IMR Import
============

Before running open settings.json and fill the blanks. You also need to add
the to settings.php the information of your Drupal 6 database.

### Example:

<code>
$databases['drupal6'] = array(
    'default' => array(
      'database' => 'db_name',
      'username' => 'your_user',
      'password' => 'your_password',
      'host' => '127.0.0.1',
      'port' => '',
      'driver' => 'mysql',
      'prefix' => 'prefix_',
  ),
);
</code>

<code>
 $databases['drupal6'] = array(
   'default' => array(
     'database' => 'db_name',
     'username' => 'your_user',
     'password' => 'your_password',
     'host' => 'localhost',
     'port' => '',
     'driver' => 'mysql',
     'prefix' => array(
       'default'   => 'prefix_',
       'authmap' => 'shared_db.shared_prefix_',
       'profile_fields' => 'shared_db.shared_prefix_',
       'profile_values' => 'shared_db.shared_prefix_',
       'sequences' => 'shared_db.shared_prefix_',
       'sessions' => 'shared_db.shared_prefix_',
       'users' => 'shared_db.shared_prefix_',
     ),
   ),
 );
</code>

To run the script `drush scr import-hubs.php`

