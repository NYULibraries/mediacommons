  <div class="<?php print $classes; ?>"<?php print $attributes; ?>>

    <div class="article-header">
      

      <?php if ($title): ?><div id="titlebar" class="titlebar"><span class="icon book"></span><h1 class="title" id="page-title"><?php print truncate_utf8( t($title), 90, TRUE, TRUE); ?></h1></div><?php endif; ?>
      <?php print render($title_suffix); ?>
      <?php echo render($content['field_field_period']); ?>
      <div class="author">
        Curated by <?php print $user_picture; ?><?php print $name; ?> 
      </div>

    </div>

    <?php print $title_safe; ?>



    <div class="content">
      <div class="image">
        <?php print $safe_tease; ?>
      </div>
      <div class="description">
        
        <?php print $description; ?>
        
        <div class="tease">
          <?php print $safe_tags; ?>      
        </div>

        <div class="trigger"><a href="javascript:void(0);" class="viewer">Articles &amp; Contributors</a></div>
          <div class="associated" style="display:none">
            <div class="contributors">
              <?php print $contributors; ?>
            </div>  
            <?php print $spokes; ?>
          </div>

        </div>

      </div>
    
  </div>