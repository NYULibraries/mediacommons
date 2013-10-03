<!DOCTYPE html>
<html dir="<?php print $language->dir; ?>" class="<?php print $classes; ?>">
<head>
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <link href='http://fonts.googleapis.com/css?family=Merriweather:700,400,900' rel='stylesheet' type='text/css'>  
  <?php print $scripts; ?>
  <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.5.2/jquery.min.js"></script>
    <script type="text/javascript">
      $(document).ready(function(){
        
        $(".associated").hide();

        $(".trigger").click(function(){
          $(this).toggleClass("active").next().slideToggle("medium");
        });

      });
    </script>  
</head>
<body class="pane body tne" <?php print $attributes;?>>
  <div id="skip-link"><a href="#main-content" id="skip-to-main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a></div>
  <section class="mediacommons-network">
    <ul class="desktop">
      <li><a href="#"><img src="<?php echo $GLOBALS['base_url'].'/sites/mediacommons.futureofthebook.org/files/mediacommons_new.png' ?>"></a></li>
      <li><a href="#">In Media Res</a></li>
      <li><a href="#">MediaCommons Press</a></li>
      <li><a href="http://dev-dl-pa.home.nyu.edu/mediacommons/" class="active">The New Everyday</a></li>
      <li><a href="#">#Alt-Academy</a></li>

      <?php
      if ($user->uid > 0) {
          print '<li class="login">' . l(t('logout'), 'user/logout') . '</li>';
      }
      else {
          print '<li class="login">' . l(t('login'), 'user'). '</li>';
      }
      ?>

    </ul>
    <ul class="mobile">
      <li><a href=""><img src="<?php echo $GLOBALS['base_url'].'/sites/mediacommons.futureofthebook.org/files/mediacommons_new.png' ?>"></a></li>
      <li class="login"><a href="#">Login</a></li>
    </ul>
  </section>
  <?php print $page_top; ?>
  <?php print $page; ?>
  <?php print $page_bottom; ?>
</body>
</html>