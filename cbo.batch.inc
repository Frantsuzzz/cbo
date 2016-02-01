<?php

/**
 * Process comment update in batch
 */
function cbo_batch_callback($callback, $cids, &$context) {
  $limit = variable_get('cbo_batch_size', 10);

  if (empty($context['sandbox']['progress'])) {
    // Текущее и Общее количество обработанных комментариев
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($cids);
  }

  if (empty($context['sandbox']['items'])) {
    $context['sandbox']['items'] = $cids;
  }

  if (!empty($context['sandbox']['items'])) {
    // Убираем из массива с данными уже обработанные комментарии
    if ($context['sandbox']['progress'] != 0) {
      array_splice($context['sandbox']['items'], 0, $limit);
    }

    //Новая партия комментариев для обработки
    $cids_part = array_slice($context['sandbox']['items'], 0, $limit);
    //обрабатываем пачку комментариев
    call_user_func($callback, $cids_part);

    $context['sandbox']['progress'] += count($cids_part);
    $context['message'] = t('Now processing comment %progress of %count', array(
      '%progress' => $context['sandbox']['progress'],
      '%count' => $context['sandbox']['max']
    ));
    $context['results']['processed'] = $context['sandbox']['progress'];
  }

  // Проверка, не пора ли закончить обработку данных.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Finish callback for Batch
 */
function cbo_batch_finished_callback($success, $results, $operations) {
  if ($success) {
    $message = format_plural($results['processed'], '@count comment processed.', '@count comments processed.');
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
}
