
<div class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

<div class="article-header">
  <?php echo render($content['field_field_period']); ?>
  <div class="social">
      <a href="#" class="share"><img src="<?php echo $GLOBALS['base_url'].'/sites/mediacommons.futureofthebook.org/themes/mediacommons_base/images/logos/facebook.png' ?>"></a>
      <a href="#" class="share"><img src="<?php echo $GLOBALS['base_url'].'/sites/mediacommons.futureofthebook.org/themes/mediacommons_base/images/logos/twitter.png' ?>"></a>
      <?php
      if ($user->uid > 0) {

      }
      else {
          print '<div class="comment_login">You must ' . l(t('login'), 'user'). ' to comment</div>';
      }
      ?>
    </div>
  <?php if ($title): ?><div id="titlebar" class="titlebar"><span class="icon book"></span><h1 class="title" id="page-title"><?php print truncate_utf8( t($title), 90, TRUE, TRUE); ?></h1></div><?php endif; ?>
    <?php print render($title_suffix); ?>
    <?php if ($display_submitted): ?>
      <div class="meta submitted">
        <?php print $user_picture; ?>
        <?php print $submitted; ?>
      </div>
    <?php endif; ?>
  </div>
  
  <?php print $title_safe; ?>
  
  <div class="content clearfix"<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
  
  <?php
    // Remove the "Add new comment" link on the teaser page or if the comment
    // form is being displayed on the same page.
    if ($teaser || !empty($content['comments']['comment_form'])) {
      unset($content['links']['comment']['#links']['comment-add']);
    }
    // Only display the wrapper div if there are links.
    $links = render($content['links']);
    if ($links):
  ?>
    <div class="link-wrapper">
      <?php print $links; ?>
    </div>
  <?php endif; ?>
  </div>
  
  <?php print render($content['comments']); ?>

</div>
