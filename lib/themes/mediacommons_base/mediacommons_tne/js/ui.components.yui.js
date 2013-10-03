/**
 * Tasks: 
 * 
 * @DONE: loading pane does not hide after first page is loaded
 * @DONE Nov 1, 2012: extend lazy load to work with X axis, various fixes.
 * @DONE Nov. 1, 2012: regresion - thumbnails toggle button (@navbar) is no longer listening for button:on/off events *
 * @DONE Nov. 2, 2012: button events listen and can be trigger by N targets *
 * @DONE Nov. 2, 2012: extend/add hover event to Slider
 * @DONE Nov. 2, 2012: on slider hover display page information
 * @DONE: keyboards events: Shift + Arrow, can be use to change to previous/next page (test).
 *
 * @TODO: support on click slider rail event
 * @TODO: cloning fist and last page from a pjax request fails in silent
 * @TODO: find a real solution for hiding loading pane, this is just a timer and does not truly represent when a book page is loaded
 * @TODO: deal with on resize events? do we need/want this?
 *
 */

Y.applyConfig({
    modules: {
        'dlts-carousel': {
            fullpath: Y.DLTS.settings.book.theme_path + 'js/modules/dlts-carousel.js'
        },
        'template-micro': {
            fullpath: Y.DLTS.settings.book.theme_path + 'js/modules/template-micro.js'
        }
    }
});

Y.use('node', 'event', 'event-custom', 'transition', 'slider', 'pjax', 'gallery-soon', 'imageloader', 'dlts-carousel', 'template', 'widget-anim', 'overlay', function (Y) {

    'use strict';

    /** definition list start */
    var 

    /** nodes */
    slider_datasource, pane_pagemeta, pane_top,

    /** callbacks */
    slide_value_change, pager_form, on_button_click, slide_end, change_page, pjax_navigate, pjax_callback, pjax_load, init_carousel, on_mousemove_over_slider_rail, on_mouseleave_slider_rail,

    /** widgets and others */
    pjax, carousel, slider, tooltip, tooltip_waiting_to_show,

    /** the object that represent the current book and settings */
    book; 

    /** definition list end */

    /** book global settings */
    book = Y.DLTS.settings.book;

    pane_top = Y.one('#top');

    pane_pagemeta = Y.one('.pane.pagemeta');

    slider_datasource = Y.one('#slider_value');

    tooltip_waiting_to_show = false;

    /** add view port information to global setting */
    book.viewport = Y.DOM.viewportRegion();

    init_carousel = function(e) {

        e.preventDefault();

        var datasource_conf, request_success;
        
        request_success = function(datasource) {
            var template, columns, columns_width, columns_max_width, columns_min_width, node, register_images, register_image, images_fold_groups, l;

            /** maximum width of each column */
            columns_max_width = 180;

            /** minimum width of each column */
            columns_min_width = 180;

            /** element to append thumbnails */
            node = Y.one('ol.modal-item');

            /** object to hold images group by custom trigger event name */
            images_fold_groups = {};

            /** columns per carousel page; the weird looking math is a fancy/faster way to achieve Math.floor() */
            columns = (book.viewport.width/columns_max_width)|0;

            /** given the view port try to calculate the optimal width of each column */
            columns_width = (book.viewport.width/columns)|0 - 10 * columns; /** 10 = padding */

            /** @TODO: this is ugly and syntactically uglier because we need a better solution */
            if (columns_width > columns_max_width || columns_width < columns_min_width ) ( ( columns_width = columns_max_width ) && ( columns = columns - 1 ) );

            /** add thumbnails configuration to global setting, columns is used when instantiating the carousel object */
            book.thumbnails = { columns: columns,  width: columns_width };

            /** compiled template */
            template = Y.Template.Micro.compile(book.templates.scrollview);

            /** length of the data */
            l = datasource.data.length;

            /** iterate datasource.data (images), this only exist because in IE and Opera images fail to register if the cache object is empty */
            register_images = function(images) {
                Y.Array.each(images, function(image) {
                    register_image(image);
                });
            };

            /** add to the images group a new image */
            register_image = function(image) {
                images_fold_groups[image.eventTrigger].registerImage({
                    domId: 'thumb-' + image.sequence, 
                    srcUrl: image.service,
                    setVisible: true
                });
            };        

            Y.Array.each(datasource.data, function(item, i) {

                var group, event_trigger;

                /** the group this image will belong */
                group = parseInt(i / columns, 10);

                event_trigger = 'image:load:group_' + group;                

                /** if group does not exist, create a new image group and add a custom trigger */
                if (group === (i / columns)) {
                    images_fold_groups[event_trigger] = new Y.ImgLoadGroup().addCustomTrigger(event_trigger);
                }

                /** assign identifier property to this item, it will be use to construct the URL */
                item.identifier = book.identifier;

                item.group = group;

                /** assign width of the image */
                item.width = columns_width; /** @TODO: make sure we asked for the best possible images depending in the available view port */

                /** assign the event that will trigger this image to load */
                item.eventTrigger = event_trigger;

                /** append the new content to the carousel */
                node.append(template(item));

                /** sad, but we need to do this in order the to successfully register images in IE and Opera when the cache object is not empty */
                if (!Y.UA.ie && !Y.UA.opera) {
                    register_image(item);
                }

                else if (i === l - 1) {
                    register_images(datasource.data);
                }

                /** we have everything we need; fire the event that will render the carousel  */
                if (i === l - 1) {
                    Y.on('contentready', function() {
                        Y.fire('carousel:render', datasource.e);            
                    }, 'ol.modal-item');
                }

            });
            
            /** listen for new images and register them in foldGroup */
            Y.on('image:available', register_image);

            /** listen for new images and register them in foldGroup */
            Y.on('images:available', register_images);        
            
        };        
        
        /** data source configuration object */ 
        datasource_conf = {
            id: 'pages-' + book.identifier,
            source: book.path  + '/book-pages.json',
               plugins: {
                   cache: {
                       sandbox: book.identifier,
                       expires: 21600000
                   },
                   json: {
                       schema: {
                           resultListLocator: 'pages',
                           resultFields: ['title', 'sequence', 'service']
                       }
                }
            }
        };
        
        /** create a new data source instance */
        Y.fire('datasource:new', datasource_conf);
        
        /** request books in collection and append to pane on success */
        Y.fire('datasource:request', {
            id: 'pages-' + book.identifier,
            request: '?limit=0',
            callback: request_success
        });
        
        Y.fire('button:button-thumbnails:on', e);

    };

    Y.once('carousel:render', function(e) {
        var carousel, scroll;

        carousel = this;

        scroll = function() {

            var sequence, current_page, selected_thumb, image_load_event_prefix;
            
            image_load_event_prefix = 'image:load:group_';
            
            sequence = parseInt(book.sequence_number, 10);

            selected_thumb = Y.one('.selected-thumb');

            /** unselect selected item */
            if (selected_thumb) {
                selected_thumb.removeClass('selected-thumb');
            }

            /** select current item */
            Y.one('.carousel-item-' + sequence).addClass('selected-thumb');

            /** locate the page for the current carousel page */
            current_page = carousel.getPageForItem(sequence - 1);
            
            /** scroll to the carousel page that includes the current book page */
            carousel.scrollToPage(current_page);

        };        
        
        carousel = new Y.DLTSCarousel({
            boundingBox: '#carousel-container',
            contentBox: '.carousel-container > ol',
            numVisible: book.thumbnails.columns,
            isCircular: false
        });

        /** listen for DLTSCarousel afterPageScroll event and fire image:load event to be listen by image N fold group */
        carousel.on('afterPageScroll', function(e) {
            Y.fire('image:load:group_' + e.pos);
        });
        
        /** synchronize the carousel, make sure current page is selected and visible */
        Y.on('openlayers:change', scroll, carousel);

        /** render the carousel */
        carousel.render();
        
        /** scroll to "current page" page in  the carousel and select the page. */
        scroll();

        /** remove from here and add to theme */
        Y.one('.yui3-carousel-nav').append('<span class="yui3-carousel-button yui3-carousel-close thumb-actions"><a href="#" class="target button thumbnails on" data-target="button-thumbnails"><em>close</em></a></span>');

    }, carousel);

    /** callbacks */
    on_button_click = function(e) {

        e.preventDefault();

        var self, event_prefix, event_id, current_target, node_target, data_target;

        self = this;

        current_target = e.currentTarget;

        /** if current target has target, get target from data-target */
        if (current_target.hasClass('target')) {
          
            data_target = self.getAttribute('data-target');

            event_prefix = 'button:' + data_target;
            
            /** look-up for the main target */
            node_target = Y.all('#' + data_target);

        }
        
        /** current target is the main target */
        else {

            event_id = self.get('id');
            
            event_prefix = 'button:' + event_id;
            
            /** find possible reference targets to this target */
            node_target = Y.all('a[data-target=' + event_id + ']');
        }

        if (self.hasClass('on')) {
            self.removeClass('on');
              if (Y.Lang.isObject(node_target)) {
                node_target.each(function(node) {
                    node.removeClass('on');
                });
              }
            Y.fire(event_prefix + ':off', e);
        }
        else {
            self.addClass('on');
              if (Y.Lang.isObject(node_target)) {
                node_target.each(function(node) {
                    node.addClass('on');
                });
              }
            Y.fire(event_prefix + ':on', e);
        }

        Y.fire(event_prefix + ':toggle', e);
    }; 
    
    /** TODO: I don't like this, find a more elegant solution */
    pager_form = function(e) {
        e.preventDefault();
        
        var value, current, path, klass; /** class is a protected keyword */

        value = this.get('value');
        
        current = parseInt(book.sequence_number, 10);
        
        path = book.path + '/';

        if (value.match(/\D/)) {
            klass = 'error';
        }    
        else {
            value = parseInt(value, 10);
            
            if (value !== current && (value > 0 && value <= book.sequence_count)) {
                klass = 'ok';
                pjax.navigate(path + value);
            }
            else {
                if (value !== current) {
                    klass = 'error';
                }
                else {
                    klass = 'warning';
                }
            }
        }
        
        this.addClass(klass).transition({
            duration: 1,
            easing: 'ease-in',
            opacity: 0.9
        }, function() {
            this.removeClass(klass);
        });
    };
    
    /** callback for changes in the value of the slider */
    slide_value_change = function(e) {
      
        var slider = this.slider, datasource = this.datasource;
      
        /** slider event */
        if (!Y.Lang.isValue(slider.triggerBy)) {
            datasource.set('value', e.newVal);
        }
        /** event was triggered by reference */
        else {
           slider.triggerBy = undefined;
        }
    };
    
    /** callback for the slide end event */
    slide_end = function(e) {
      
        e.preventDefault();
        
        var slider, url;
        
        slider = this;

        url = book.path + '/' + e.target.getValue();
        
        if (!Y.Lang.isValue(slider.triggerBy)) {

            pjax.navigate(url);

            /** slider set focus to the slider rail, blur as soon as possible so that user can use the keyboard to read the book */
            Y.soon(function () {
                slider.thumb.blur();
            });

        }
        
        /** event was triggered by reference */
        else {
          slider.triggerBy = undefined; 
        }
    };
    
    change_page = function(config) {
        var map, service, zoom, open_layers_dlts;
        
        open_layers_dlts = OpenLayers.DLTS;

        if (Y.Lang.isObject(open_layers_dlts.pages[0], true)) {
            map =  open_layers_dlts.pages[0];
            service = map.baseLayer.url;
            zoom = map.getZoom();
            map.destroy();
            open_layers_dlts.pages = [];
        }

        if (Y.Object.isEmpty(open_layers_dlts.pages)) {
            open_layers_dlts.Page(config.id, config.uri, { 
                zoom: zoom,
                boxes: config.boxes,
                service: service,
                imgMetadata: config.metadata
            });
            
            Y.on('contentready', function() {
                Y.one('#page-title').setContent(config.title);
                Y.fire('openlayers:change', config);
                Y.later(3000, Y.one('.pane.load'), function() {
                    this.hide();
                });
            }, '#' + config.id);
        }
    };

    pjax_navigate = function(e) {
        var msg = e.url.replace(book.path + '/', '');
        if (/(^[\d]+$){1}/.test(msg)) {
            this.one('.current_page').set('text', msg);
        }
        else if (/(^[\d]+-[\d]+$){1}/.test(msg)) {
            this.one('.current_page').set('text', msg);
        }
        else {
            this.one('.current_page').set('text', '');
        }
      this.addClass('loading');
      this.show();
    };
    
    /** pjax callback can be call by clicking a pjax enable link or by reference with data-url */  
    pjax_callback = function(e) {
        e.preventDefault();
        
        var url;
        
        /** if this has referenceTarget, then this event was trigger by reference*/
        if (Y.Lang.isObject(e.referenceTarget, true)) {
            url = e.referenceTarget.getAttribute('data-url');
        }
        
        /** trigger by a pjax enable link */
        else {
            url = this.get('href');
        }
      
        pjax.navigate(url);
    };
    
    pjax_load = function(e) {
        
        var node, map, next, prev, toogle, btn_paging, btn_pager, btn_metadata, btn_pagings, btn_fullscreen, sequence, sequence_count, page_type, currentThumb, shapes, boxes, config;

        node = e.content.node;
        
        btn_fullscreen = Y.one('a.fullscreen');
        
        btn_pager = Y.one('#pager');
        
        btn_metadata = Y.one('a.metadata');
        
        btn_pagings = Y.all('.paging.button a')
        
        btn_paging = Y.all('.paging.button');
        
        map = node.one('.dlts_image_map');
        
        next = node.one('.next');
        
        prev = node.one('.previous');
        
        toogle = node.one('.toogle');

        page_type = node.getAttribute('data-type');
        
        sequence = parseInt(node.getAttribute('data-sequence'), 10);
        
        sequence_count = parseInt(node.getAttribute('data-sequence-count'), 10);

        currentThumb = Y.one('#thumb-' + sequence);
        
        /** remove from here ?*/
        shapes = Y.one('#shapes');
        
        /** remove from here ?*/
        boxes = [];
        
        /** remove from here ?*/
        if (shapes) {
              shapes = JSON.parse(shapes.get('text'));
            boxes = shapes.shapes.ocr;
        }
        
        book.sequence_number = sequence;
        
        /** At this point we already have toogle/next/previous, replace the old ones */ 
        Y.one('.paging.previous').replace(prev.cloneNode(true));
        Y.one('.paging.next').replace(next.cloneNode(true));
        Y.one('a.toogle').replace(toogle.cloneNode(true));
        
        /** Configuration for the new map */
        config = {
            id: map.get('id'),
            title: node.getAttribute('data-title'),
            node: map,
            boxes: boxes,
            sequence: node.getAttribute('data-sequence'),
            uri: map.getAttribute('data-uri'),
            metadata: {
                width: map.getAttribute('data-width'),
                height: map.getAttribute('data-height'),
                levels: map.getAttribute('data-levels'),
                dwtLevels: map.getAttribute('data-dwtlevels'),
                compositingLayerCount: map.getAttribute('data-compositing-layer')
            }
        };

        Y.on('available', change_page, '#' + config.id, OpenLayers, config);

    };

    /** handler that positions and shows the tooltip */ 
    on_mousemove_over_slider_rail = function (e) {

        var slider, tooltip, rail_value, coordinates_to_move;

        slider = this.slider;

        tooltip = this.tooltip;

        if (slider.axis === 'x') {
            rail_value = slider._offsetToValue(e.clientX);
        }
        else {
            rail_value = slider._offsetToValue(e.clientY);
        }
        
        /** get X coordinate from event pageX and Y since is fixed, take the value from the size of the view port minus the computed height of the pager and the tooltip message */
        coordinates_to_move = [ (e.pageX ), (book.viewport.height - 65) ];

        if (tooltip.get('visible') === false) {
            /** while it's still hidden, move the tooltip adjacent to the cursor */
            Y.one('.tooltip').setStyle('opacity', '0');
            tooltip.move(coordinates_to_move);
        }

        if (tooltip_waiting_to_show === false) {
            /** wait half a second, then show tooltip */
            setTimeout(function(){
                Y.one('.tooltip').setStyle('opacity', '1');
                tooltip.show();
            }, 500);
                  
            /** while waiting to show tooltip, don't let other mousemoves try to show tooltip too. */
            tooltip_waiting_to_show = true;
        }
            
        tooltip.move(coordinates_to_move);
        tooltip.setStdModContent('body', rail_value);
    };

    /** handler that hides the tooltip after the mouse leave the slider rail */
    on_mouseleave_slider_rail = function (e) {
        var self = this;
        
        /** this check prevents hiding the tooltip when the cursor moves over the tooltip itself */ 
        if ((e.relatedTarget) && (e.relatedTarget.hasClass('yui3-widget-bd') === false)) {
            self.hide();
            tooltip_waiting_to_show = false;            
        }
    };
    
    /** tooltip object */    
    tooltip = new Y.Overlay({
        srcNode: ".tooltip",
        visible: false
    });
    
    /** pjax object */    
    pjax = new Y.Pjax({ container: '.pane.display' });
    
    /** slider object */
    slider = new Y.Slider({
        axis: 'x',
        min: 1,
        clickableRail: false,
        max: parseInt(book.sequence_count, 10),
        value: parseInt(book.sequence_number, 10),
        length: (book.viewport.width - 150) + 'px',
        thumbUrl: '/themasses/sites/dlib.nyu.edu.themasses/themes/dlts_book/images/slider/thumb-x.png'
    });

    /** render the slider and plug-ins */
    
    slider.render('#slider');
    
    tooltip.plug(Y.Plugin.WidgetAnim);
          
    tooltip.anim.get('animHide').set('duration', 0.01);
        
    tooltip.anim.get('animShow').set('duration', 0.3);
        
    tooltip.render('#slider');

    /** events listeners */
   
    slider.after('valueChange', slide_value_change, { datasource: slider_datasource, slider: slider });

    slider.after('slideEnd', slide_end, slider);
    
    slider.rail.on('mousemove', on_mousemove_over_slider_rail, { slider: slider, tooltip: tooltip });
    
    slider.rail.on('mouseleave', on_mouseleave_slider_rail, tooltip);
    
    slider.after('thumbMove', function(e) {});
    
    Y.on('openlayers:next', pjax_callback, Y.one('.paging.next'));
    
    Y.on('pjax:change', pjax_callback);
    
    Y.on('openlayers:previous', pjax_callback, Y.one('.paging.previous'));
    
    Y.one('.pane.pager').delegate('submit', pager_form, 'form', slider_datasource);

    pjax.on('load', pjax_load);

    pjax.on('navigate', pjax_navigate, Y.one('.pane.load'));
    
    Y.one('#page').delegate('click', on_button_click, 'a.button');

    Y.one('#page').delegate('click', pjax_callback, 'a.paging, a.toogle');

    Y.on('button:button-metadata:on', function(e) {
          this.removeClass('hidden');
          this.ancestor('.pane-body').removeClass('pagemeta-hidden');
    }, pane_pagemeta);
    
    Y.on('button:button-metadata:off', function(e) {
          this.addClass('hidden');
          this.ancestor('.pane-body').addClass('pagemeta-hidden');
    }, pane_pagemeta);

    Y.on('button:button-fullscreen:on', function(e) {
        Y.fire('button:' + this.button.get('id') + ':off', this.pagemeta);
        this.button.removeClass('on');
        this.top.addClass('hidden');
    }, {  
        top: pane_top,
        pagemeta: pane_pagemeta,
        button: Y.one('a.metadata')
   });

    Y.on('button:button-fullscreen:off', function(e) {
        this.removeClass('hidden');
    }, pane_top);

    Y.on('openlayers:change', function(config) {
        
        var sequence, slider, datasource;
        
        sequence = config.sequence
        
        datasource = this.datasource;
        
        slider = this.slider;
        
        slider.triggerBy = 'openlayers:change';
        
        datasource.set('value', sequence);
        
        slider.set('value', sequence);
        
    }, {
        datasource: slider_datasource,
        slider: slider
    });
    
    Y.on('button:button-thumbnails:on', function(e) {
        this.removeClass('hidden');
    }, Y.one('.yui3-carousel-container'));
    
    Y.on('button:button-thumbnails:off', function(e) {
        this.addClass('hidden');
    }, Y.one('.yui3-carousel-container'));
    
    /** Set-up the carousel */
    Y.once('click', init_carousel, '.button.thumbnails');
    
    /** Hide loading pane after page is loaded */
    Y.once('contentready', function() {
        Y.later(3000, Y.one('.pane.load'), function() {
            this.hide();
        });
    }, '.dlts_image_map');

});