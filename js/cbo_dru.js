/**
 * @file
 * Adjust node details block width for screen size.
 */

(function ($, Drupal) {
    "use strict";

    /**
     * Provide node details affix feature.
     */
    Drupal.behaviors.AlphaNodeAffix = {
        attach: function (context) {

            $(".cbo-form").width($(".cbo-form").parent().width());

            $(".cbo-form").affix({
                offset: {top: function () {
                    console.log(this);
                    // возвращает координату Y родительского .node
                    var node_parent = $(".cbo-form").closest('.node');

                    return (this.top = $(".cbo-form").offset().top)
                },
                    bottom: function () {
                        // возвращает расстояние от нижней точки родительского .node до низа окна
                        var node_parent = $(".cbo-form").closest('.node');
                        var all_height = $(document).height();
                        return (this.bottom = all_height - node_parent.offset().top - node_parent.height())
                    }
                }
            });

            $(window).on('resize', function () {
                $(".cbo-form").width($(".cbo-form").parent().width());
            });
        }
    };

})(jQuery, Drupal);
