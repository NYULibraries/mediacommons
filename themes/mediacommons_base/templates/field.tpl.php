<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php print $safe_label; ?>
  <?php foreach ($items as $delta => $item) : ?>  
    <div class="yui3-u"><?php print render($item); ?></div>
  <?php endforeach; ?>
</div>