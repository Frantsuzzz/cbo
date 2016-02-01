(function ($) {
    Drupal.cbo = Drupal.cbo || {};


    // Меняем состояние всех чекбоксов на странице
    Drupal.cbo.editChildrensCheck = function (nid, checked) {
        var childrens = $('body').find('input.cbo-select-' + nid);
        childrens.prop("checked", checked).attr('checked', checked);
    };


    // Количество всех чекбоксов
    Drupal.cbo.countChildrens = function (nid, status) {
        if (status !== 'all' && status !== 'checked') {
            return 0;
        }

        if (status == 'all') {
            var childrens = $('body').find('input.cbo-select-' + nid);
        } else {
            var childrens = $('body').find('input.cbo-select-' + nid + ':checked');
        }

        return childrens.length;
    };


    //Заносим данные о выбранных элементах на форму
    Drupal.cbo.updateSelectedItems = function (nid) {
        var form = $('.cbo-form-' + nid);
        if (form.length <= 0) {
            return;
        }

        var check_all = form.find('input[name="cbo_check_all"]').first();
        var items = form.find('input[name=cbo_items]');

        if (check_all.is(':checked')) {
            //значит выбраны все комментарии
            items.val('');
            //Обновим сообщение
            $msg = Drupal.t('Select all comments for node');
            Drupal.cbo.updateCountMsg(nid, $msg, 'warning');
        } else {
            //находим все отмеченные элементы

            var selectedItems = new Array();
            $('body').find('input.cbo-select-' + nid + ':checked').each(function () {
                selectedItems.push($(this).val());
            });

            var count_checked = selectedItems.length;
            if (count_checked > 0) {
                items.val(selectedItems.join(';'));
                //Сообщение
                var msg = Drupal.formatPlural(count_checked, 'Set @count comment', 'Selected @count comments');
                Drupal.cbo.updateCountMsg(nid, msg, '');

            } else {
                items.val('');
                var msg = Drupal.t('Selected 0 comment');
                Drupal.cbo.updateCountMsg(nid, msg, '');
            }

            //Добавим класс к .comment
            $('body').find('input.cbo-select-' + nid + ':not(:checked)').closest('.comment').removeClass('cbo-selected');
            $('body').find('input.cbo-select-' + nid + ':checked').closest('.comment').addClass('cbo-selected');


        }


    };


    //Вывод сообщения
    Drupal.cbo.updateCountMsg = function (nid, msg, attr_class) {
        var log = $('#cbo-log-' + nid);
        var this_class = 'cbo-msg ' + attr_class;
        log.html('<span class="' + this_class + '">' + msg + '</span>');
    };


    Drupal.behaviors.cbo = {
        attach: function (context, settings) {

            //Форма: повесим обработчик на чекбоксы
            $('.cbo-form:not(.cbo-processed)').addClass('cbo-processed').each(function () {
                var check = $(this).find('input[name="cbo_check"]');
                var check_all = $(this).find('input[name="cbo_check_all"]');
                var nid = $(this).data('nid');

                //Отметить/Снять вce комментарии на странице
                check.change(function () {
                    var nid = $(this).data('nid');
                    Drupal.cbo.editChildrensCheck(nid, this.checked);

                    var check_all = $('.cbo-form-' + nid).find('input[name="cbo_check_all"]');
                    check_all.prop("checked", false);

                    Drupal.cbo.updateSelectedItems(nid);
                    return false;
                });

                //Отметить/Снять вce комментарии к ноде
                check_all.change(function () {
                    var nid = $(this).data('nid');
                    Drupal.cbo.updateSelectedItems(nid);
                    return false;
                });

            });

            $('.cbo-form').each(function () {
                Drupal.cbo.updateSelectedItems($(this).data('nid'));
            });

            //Комментарий: повесим обработчик на чекбокс
            $('input.cbo-select:not(.cbo-processed)').addClass('cbo-processed').change(function () {
                var nid = $(this).data('nid');

                //Нужно пересчитать отмеченные чекбоксы на странице
                var count_all = Drupal.cbo.countChildrens(nid, 'all');
                var count_checked = Drupal.cbo.countChildrens(nid, 'checked');
                var form = $('.cbo-form-' + nid);
                var check = form.find('input[name=cbo_check]');
                var check_all = form.find('input[name=cbo_check_all]');

                if (count_checked == count_all) {
                    check.prop("checked", true);
                    check_all.parent().show();
                } else {
                    check.prop("checked", false);
                    check_all.parent().hide();
                }

                check_all.prop("checked", false);

                Drupal.cbo.updateSelectedItems(nid);
                return false;
            });


        }};

})(jQuery);
