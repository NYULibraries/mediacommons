<div class="pane page">
  <div id="page-inner" class="page-inner">
  <a id="main-content"></a>
    
    <div id="top" class="pane top">
      
      <section class="header">    
        <div class="wrapper">
          <img src="<?php echo $GLOBALS['base_url'].'/sites/mediacommons.futureofthebook.org/themes/mediacommons_base/images/logos/tne_new.png' ?>">
          <div id="search">
            <?php print render($page['search']); ?>
          </div>
          <div class="content-nav">
            <?php print render($page['navigation']); ?>
          </div>
          <?php if ($logo): ?>
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /></a>
          <?php endif; ?>      
          <!--<?php if ($site_name) : ?>
            <<?php if ($title) : print 'h1'; else: print 'h2'; endif; ?> id="site-name"><a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a></<?php if ($title) : print 'h1'; else: print 'h2'; endif; ?>>
          <?php endif; ?>-->


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
          
          <!--<?php if ($breadcrumb) : ?>
            <div id="breadcrumb"><?php print $breadcrumb; ?></div>
          <?php endif; ?>-->
          
          <?php if ($messages) : ?>
            <?php print $messages; ?>   
          <?php endif; ?>
          
          <?php if (isset($page['highlighted'])) : ?>
            <div id="highlighted"><?php print render($page['highlighted']); ?></div>
          <?php endif; ?>

        </div>
      </section>
      
      <div class="wrapper">    
        
        <?php print render($title_prefix); ?>
        
        
        <?php print render($page['help']); ?>
        
        <?php if ($action_links): ?>
          <ul class="action-links"><?php print render($action_links); ?></ul>
        <?php endif; ?>
      </div>
    </div>
    <div class="wrapper the-content">
      <?php if ($tabs) : ?>
          <div class="tabs"><?php print render($tabs); ?></div>
        <?php endif; ?>
    <?php print render($page['content']); ?>
    </div>
    <?php if ($feed_icons) : ?>
      <?php print $feed_icons; ?> 
    <?php endif; ?>
    
  </div>

</div>
<?php if (isset($page['footer']) && !empty($page['footer'])) : ?>
      <div class="footer yui3-g wrapper">
        <div class="yui3-u-1">  
            <?php print render($page['footer']); ?>
            MediaCommons: A Digital Scholarly Network -- Powered by NYU
        </div>
    </div>
    <?php endif; ?>