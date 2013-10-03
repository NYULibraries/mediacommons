<div id="comments" class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php if ($content['comments'] && $node->type != 'forum'): ?>
    <?php print render($title_prefix); ?>
    <h4 class="title"><?php print t('Comments'); ?></h4>
    <?php print render($title_suffix); ?>
  <?php endif; ?>

  <?php
  if ($user->uid > 0) {
  }
  else {
      print '<div class="must-login"><p>You must be logged in to comment.</p><p>' . l(t('login'), 'user'). '</p></div>';
  }
  ?>

  <?php if ($content['comment_form']): ?>
    <?php print render($content['comment_form']); ?>
  <?php endif; ?>
  <?php print render($content['comments']); ?>
</div>
