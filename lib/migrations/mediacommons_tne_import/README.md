MediaCommons TNE Import
============

Copy default.settings.json as settings.json and fill the blanks. Also, add to your site settings.php the information of the Drupal 6 database(s) that will be migrated.

### Example:

 ```php
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
```

### Shared DB example: 

```php
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
```

To run the script `drush scr mediacommons_tne_import.module`

