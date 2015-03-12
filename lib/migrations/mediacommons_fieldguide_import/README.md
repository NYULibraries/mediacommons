MediaCommons MC Import
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
       'default'   => 'default_',
       'authmap' => 'shared.shared_',
       'profile_fields' => 'shared.shared_',
       'profile_values' => 'shared.shared_',
       'realname' => 'shared.shared_',
       'sequences' => 'shared.shared_',
       'term_data' => 'shared.shared_',
       'term_relation' => 'shared.shared_',
       'term_synonym' => 'shared.shared_',       
       'vocabulary' => 'shared.shared_',
       'users' => 'shared.shared_',
     ),
   ),
 );
```

To run the script `drush scr mediacommons_mc_import.module`