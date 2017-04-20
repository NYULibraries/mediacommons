core = 7.x

api = 2

projects[drupal][version] = "7.54"

; Modules

projects[apachesolr][version] = "1.8"
projects[apachesolr_multisitesearch][version] = "1.1"
projects[ctools][version] = "1.12"
projects[cnr][version] = "4.22"
projects[date][version] = "2.10"
projects[diff][version] = "3.3"
projects[ds][version] = "2.14"
projects[entity][version] = "1.8"
projects[features_extra][version] = "1.0"
projects[features][version] = "2.10"
projects[imagefield_crop][version] = "1.1"
projects[jquery_update][version] = "2.7"
projects[libraries][version] = "2.3"
projects[link][version] = "1.4"
projects[menu_token][version] = "1.0-beta7"
projects[references][version] = "2.2"
projects[nodequeue][version] = "2.1"
projects[pathauto][version] = "1.3"
projects[realname][version] = "1.3"
projects[strongarm][version] = "2.0"
projects[token][version] = "1.7"
projects[views][version] = "3.16"
projects[views_fieldsets][version] = "2.1"
projects[admin_menu][version] = "3.0-rc5"
projects[devel][version] = "1.5"
projects[field_group][version] = "1.5"
projects[email][version] = "1.3"
projects[telephone][version] = "1.0-alpha1"
projects[gravatar][version] = "1.1"
projects[ckeditor][version] = "1.16"
projects[manualcrop][version] = "1.6"
projects[uuid][version] = "1.0-beta2"
projects[better_exposed_filters][version] = "3.4"
projects[viewfield][version] = "2.1"
projects[reroute_email][version] = "1.2"
projects[admin_views][version] = "1.6"
projects[views_bulk_operations][version] = "3.4"
projects[apachesolr_comments][version] = "1.0"
projects[role_delegation][version] = "1.1"
projects[rules][version] = "2.10"
projects[field_permissions][version] = "1.0"
projects[og][version] = "2.9"

; in-house modules
projects[mediacommons_modules][type] = "module"
projects[mediacommons_modules][download][type] = "git"
projects[mediacommons_modules][download][url] = "https://github.com/NYULibraries/mediacommons_modules.git"
projects[mediacommons_modules][download][branch] = "develop"

; Profiles

projects[mediacommons_projects][type] = "profile"
projects[mediacommons_projects][download][type] = "git"
projects[mediacommons_projects][download][url] = "https://github.com/NYULibraries/mediacommons_projects.git"
projects[mediacommons_projects][download][branch] = "develop"

projects[mediacommons_umbrella][type] = "profile"
projects[mediacommons_umbrella][download][type] = "git"
projects[mediacommons_umbrella][download][url] = "https://github.com/NYULibraries/mediacommons_umbrella.git"
projects[mediacommons_umbrella][download][branch] = "develop"

; Themes

projects[rubik][version] = "4.4"
projects[tao][version] = "3.1"
projects[zen][version] = "5.6"

projects[mediacommons][download][type] = "git"
projects[mediacommons][download][url] = "https://github.com/NYULibraries/mediacommons_theme.git"
projects[mediacommons][type] = "theme"
projects[mediacommons][download][branch] = "develop"

projects[mediacommons_admin][download][type] = "git"
projects[mediacommons_admin][download][url] = "https://github.com/NYULibraries/mediacommons_admin.git"
projects[mediacommons_admin][type] = "theme"

; Libraries

libraries[simplehtmldom][download][type] = "get"
libraries[simplehtmldom][download][url] = "http://dlib.nyu.edu/files/simplehtmldom.zip"
libraries[simplehtmldom][directory_name] = "simplehtmldom"
libraries[simplehtmldom][type] = "library"

; imagesLoaded.
libraries[jquery.imagesloaded][download][type] = "get"
libraries[jquery.imagesloaded][download][url] = "https://github.com/desandro/imagesloaded/archive/v2.1.2.tar.gz"
libraries[jquery.imagesloaded][download][subtree] = "imagesloaded-2.1.2"
libraries[jquery.imagesloaded][type] = "library"

libraries[jquery.imgareaselect][download][type] = "file"
libraries[jquery.imgareaselect][download][url] = "https://github.com/odyniec/imgareaselect/archive/v0.9.11-rc.1.tar.gz"
libraries[jquery.imgareaselect][download][subtree] = "imgareaselect-0.9.11-rc.1"
libraries[jquery.imgareaselect][type] = "library"
