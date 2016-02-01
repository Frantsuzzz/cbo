<?php

function cbo_forms($form_id, $args) {
  $forms = array();
  if (strncmp($form_id, 'cbo_moderate_form_', 18) === 0) {
    $forms[$form_id] = array('callback' => 'cbo_moderate_form');
  }
  return $forms;
}


function cbo_moderate_form($form, &$form_state, $node) {

  $operations = cbo_actions_list();
  $options = array('none' => t('- Select action -'));
  foreach ($operations as $key => $op) {
    $options[$key] = $op['label'];
  }

  $form = array(
    '#attributes' => array(
      'class' => array(
        'cbo-form',
        'cbo-form-' . $node->nid,
        'clearfix',
      ),
      'data-nid' => $node->nid,
      'data-comment-count' => $node->comment_count,
    ),
  );


  $form['cbo_check'] = array(
    '#type' => 'checkbox',
    '#title' => t('All page'),
    '#default_value' => 0,
    '#description' => '',
    '#attributes' => array(
      'data-nid' => $node->nid,
    ),
  );

  $form['check'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'log-wrapper',
      ),
    ),
  );

  $form['check']['cbo_check_info'] = array(
    '#markup' => '<div id="cbo-log-' . $node->nid . '" class="cbo-log"></div>',
  );

  $form['check']['cbo_check_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('All comments'),
    '#default_value' => 0,
    '#description' => '',
    '#attributes' => array(
      'data-nid' => $node->nid,
    ),
    '#states' => array(
      // Элемент будет показан тогда, когда чекбокс check_me будет нажат.
      'visible' => array(
        ':input[name="cbo_check"]' => array('checked' => TRUE),
      ),
    ),
  );


  $form['cbo_items'] = array('#type' => 'hidden', '#default_value' => '');

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['cbo_actions'] = array(
    '#type' => 'select',
    '#title' => t('Actions'),
    '#options' => $options,
    '#default_value' => 'none',
    '#description' => '',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Start'),
  );

  $form['#attached']['css'] = array(
    drupal_get_path('module', 'cbo') . '/css/cbo.css',
  );

  $form['#attached']['js'] = array(
    drupal_get_path('module', 'cbo') . '/js/cbo.js',
    drupal_get_path('module', 'cbo') . '/js/cbo_dru.js',
  );

  return $form;
}

function cbo_moderate_form_validate($form, &$form_state) {
  $operations = cbo_actions_list();
  if (!isset($operations[$form_state['values']['cbo_actions']])) {
    form_set_error('cbo_actions', t('Error @field', array('@field' => 'cbo_actions')));
  }

//TODO добавить проверку на введенные значения cid или all

}


function cbo_moderate_form_submit($form, &$form_state) {
  $cbo_batch_size = variable_get('cbo_batch_size', 10);
  $is_all = $form_state['values']['cbo_check_all'];
  $op = $form_state['values']['cbo_actions'];
  $cids = array();

  //Подготавливаем массив из cid
  if ($is_all) {
    $nid = isset($form_state['build_info']['args'][0]->nid) ? $form_state['build_info']['args'][0]->nid : FALSE;
    if ($nid) {
      $cids = db_select('comment', 'c')
        ->fields('c', array('cid'))
        ->condition('c.nid', $nid)
        ->execute()
        ->fetchCol();
    }
  }
  else {
    $items = trim($form_state['values']['cbo_items']);
    $cids = explode(';', $items);
  }

  $count_cids = count($cids);
  if (!$count_cids) {
    return;
  }

  $operations = cbo_actions_list();
  $callback = $operations[$op]['callback'];

  if ($count_cids < $cbo_batch_size) {
    //Обработка напрямую
    call_user_func($callback, $cids);
  }
  else {
    //Обработка через batch
    // Создаём массив с данными для батча
    $batch = array(
      'title' => t('Comment processing'),
      'operations' => array(
        array('cbo_batch_callback', array($callback, $cids)),
      ),
      'finished' => 'cbo_batch_finished_callback',
      'file' => drupal_get_path('module', 'cbo') . '/cbo.batch.inc',
    );
    batch_set($batch);
    batch_process();
  }
}
