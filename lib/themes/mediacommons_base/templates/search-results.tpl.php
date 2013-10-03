<?php

/**
 * @file
 * Default theme implementation for displaying search results.
 *
 * This template collects each invocation of theme_search_result(). This and
 * the child template are dependent to one another sharing the markup for
 * definition lists.
 *
 * Note that modules may implement their own search type and theme function
 * completely bypassing this template.
 *
 * Available variables:
 * - $search_results: All results as it is rendered through
 *   search-result.tpl.php
 * - $module: The machine-readable name of the module (tab) being searched, such
 *   as "node" or "user".
 *
 *
 * @see template_preprocess_search_results()
 *
 * @ingroup themeable
 */
?>
<?php if ($search_results): ?>
	<div class="search-results-col">
	  <h4 class="bold-title"><?php print t('Search results');?></h4>
	  <div class="search-results-wrapper">
		  <ul class="search-results <?php print $module; ?>-results">
		    <?php print $search_results; ?>
		  </ul>
		</div>
	  <?php print $pager; ?>
	</div>
<?php else : ?>
	<div class="search-results-col">
	  <h4 class="bold-title none"><?php print t('Your search yielded no results');?></h4>
	  <?php print search_help('search#noresults', drupal_help_arg()); ?>
	</div>
<?php endif; ?>
