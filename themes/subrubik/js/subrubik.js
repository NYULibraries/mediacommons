/**
 * Implementation of Drupal behavior.
 */
(function($) {

  Drupal.behaviors.subrubik = {};
  
  Drupal.behaviors.subrubik.attach = function(context) {
    window.console.log('subrubik');
    
    $(".node-type-cluster").delegate("input:radio", "change", function() {
      console.log($(this).val());
    });
    
  };
  
})(jQuery);
