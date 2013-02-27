<div class="pane page">
  <div id="page-inner" class="page-inner">
  <a id="main-content"></a>
  <div id="top" class="pane top">
    <div id="header" class="header">    
    <?php print render($page['search']); ?>
      <?php if ($logo): ?>
        <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /></a>
      <?php endif; ?>      
      <?php if ($site_name) : ?>
        <<?php if ($title) : print 'h1'; else: print 'h2'; endif; ?> id="site-name"><a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a></<?php if ($title) : print 'h1'; else: print 'h2'; endif; ?>>
      <?php endif; ?>
    </div>
    
    <?php if ($main_menu || $secondary_menu) : ?>
      <div id="navigation" class="navigation">
        <?php if ($main_menu) : ?>
          <?php print theme('links__system_main_menu', array('links' => $main_menu, 'attributes' => array('id' => 'main-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => t('Main menu'))); ?>
        <?php endif; ?>
        <?php if ($secondary_menu) : ?>
          <?php print theme('links__system_secondary_menu', array('links' => $secondary_menu, 'attributes' => array('id' => 'secondary-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => t('Secondary menu'))); ?>
        <?php endif; ?>
      </div>
    <?php endif; ?>
    
    <?php if ($breadcrumb) : ?>
      <div id="breadcrumb"><?php print $breadcrumb; ?></div>
    <?php endif; ?>
    
    <?php if ($messages) : ?>
      <?php print $messages; ?>   
    <?php endif; ?>
    
    <?php if (isset($page['highlighted'])) : ?>
      <div id="highlighted"><?php print render($page['highlighted']); ?></div>
    <?php endif; ?>
    
    <?php print render($title_prefix); ?>
    
    <?php if ($title): ?><div id="titlebar" class="titlebar"><span class="icon book"></span><h1 class="title" id="page-title"><?php print truncate_utf8( t($title), 90, TRUE, TRUE); ?></h1></div><?php endif; ?>
      <?php print render($title_suffix); ?>
    <?php print render($title_suffix); ?>
    
    <?php if ($tabs) : ?>
      <div class="tabs"><?php print render($tabs); ?></div>
    <?php endif; ?>
    
    <?php print render($page['help']); ?>
    
    <?php if ($action_links): ?>
      <ul class="action-links"><?php print render($action_links); ?></ul>
    <?php endif; ?>
  </div>
  
  <?php print render($page['content']); ?>
  
  <?php if ($feed_icons) : ?>
    <?php print $feed_icons; ?> 
  <?php endif; ?>
  
  <?php if (isset($page['footer']) && !empty($page['footer'])) : ?>
    <div class="footer yui3-g">
      <div class="yui3-u-1">  
          <?php print render($page['footer']); ?>
      </div>
  </div>
  <?php endif; ?>
  </div>
</div>