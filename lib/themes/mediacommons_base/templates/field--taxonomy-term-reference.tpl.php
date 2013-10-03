<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <ul class="tags">
    <lh><strong>Tags: </strong></lh>
    <?php foreach ($items as $delta => $item) : ?>
      <li><?php print render($item); ?></li>
    <?php endforeach; ?>
   </ul>
</div>