<?php
function cbo_settings_form($form_state) {
  $form = array();

  // Загружаем все существующие типы материала
  $options = node_type_get_names();

  $form['cbo_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node types'),
    '#options' => $options,
    '#default_value' => variable_get('cbo_node_types', array()),
    '#description' => t('Display selected node types'),
  );

  $form['cbo_batch_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Batch size'),
    '#default_value' => variable_get('cbo_batch_size', 10),
    '#description' => t('MIin size for start batch'),
    '#element_validate' => array('element_validate_integer_positive'),
  );

  return system_settings_form($form);
}
