<?php
	switch(ObtenerGet('act')){
		case 'rollingdice':
			$_SESSION['DADO_FUERZA']				=	mt_rand(10,18);
			$_SESSION['DADO_AGILIDAD']			=	mt_rand(10,18);
			$_SESSION['DADO_INTELIGENCIA']	=	mt_rand(10,18);
			$_SESSION['DADO_CARISMA']				=	mt_rand(10,18);
			$_SESSION['DADO_CONSTITUCION']	=	mt_rand(10,18);
			$return = array(
												"df"	=>	$_SESSION['DADO_FUERZA'],
												"da"	=>	$_SESSION['DADO_AGILIDAD'],
												"di"	=>	$_SESSION['DADO_INTELIGENCIA'],
												"dca"	=>	$_SESSION['DADO_CARISMA'],
												"dco"	=>	$_SESSION['DADO_CONSTITUCION']
											);
			echo json_encode($return);
			exit;
		break;
		case 'changerace':
			if(Validar(ObtenerPost('race'),'num')){
				if($row = $conn->Execute("SELECT * FROM acc_races WHERE id = '".(int)ObtenerPost('race')."'")){
					if(!$row->EOF()){
						$_SESSION["add_strength"]			=	$row->add_strength;
						$_SESSION["add_agility"]			=	$row->add_agility;
						$_SESSION["add_inteligence"]	=	$row->add_inteligence;
						$_SESSION["add_charisma"]			=	$row->add_charisma;
						$_SESSION["add_constitution"]	=	$row->add_constitution;
						$return = array(
															"df"	=>	$_SESSION['add_strength'],
															"da"	=>	$_SESSION["add_agility"],
															"di"	=>	$_SESSION["add_inteligence"],
															"dca"	=>	$_SESSION["add_charisma"],
															"dco"	=>	$_SESSION["add_constitution"]
														);
						echo json_encode($return);
					}
				}
			}
			exit;
		break;
		default:
			$Filename_To_Show = DIR_MODULES."controlpanel/newplayer.html";
			$template->assign_var("NEWPLAYERJS",TEMPLATE_ROUTER.DIR_MODULES."controlpanel/newplayer.js");
			if(isset($_POST['submit'])){
				$NewPJ = ObtenerPost('NewPJ');
				$validar_campos = ValidarCamposNP($NewPJ);
				if($validar_campos == 'OK'){
					$NewPJ['gender']--;
					if($row = $conn->Execute("SELECT * FROM acc_races WHERE id = '".$NewPJ['race']."'")){
						if(!$row->EOF()){
							$_SESSION["add_strength"]			=	$row->add_strength;
							$_SESSION["add_agility"]			=	$row->add_agility;
							$_SESSION["add_inteligence"]	=	$row->add_inteligence;
							$_SESSION["add_charisma"]			=	$row->add_charisma;
							$_SESSION["add_constitution"]	=	$row->add_constitution;
						}
					}
					$Dados = array(
													"FUERZA"				=>	(int)$_SESSION['DADO_FUERZA'] + (int)$_SESSION["add_strength"],
													"AGILIDAD"			=>	(int)$_SESSION['DADO_AGILIDAD'] + (int)$_SESSION["add_agility"],
													"INTELIGENCIA"	=>	(int)$_SESSION['DADO_INTELIGENCIA'] + (int)$_SESSION["add_inteligence"],
													"CARISMA"				=>	(int)$_SESSION['DADO_CARISMA'] + (int)$_SESSION["add_charisma"],
													"CONSTITUCION"	=>	(int)$_SESSION['DADO_CONSTITUCION'] + (int)$_SESSION["add_constitution"]
												);
					$sql = "INSERT INTO acc_players (name,account_id,class,gender,race,home,strength,agility,inteligence,charisma,constitution) VALUES ('{$NewPJ['name']}','{$_SESSION["AOC_SITE_ID"]}','{$NewPJ['class']}','{$NewPJ['gender']}','{$NewPJ['race']}','{$NewPJ['home']}','{$Dados['FUERZA']}','{$Dados['AGILIDAD']}','{$Dados['INTELIGENCIA']}','{$Dados['CARISMA']}','{$Dados['CONSTITUCION']}')";
					if($conn->Execute($sql)){
						$sql = "UPDATE account SET players = players + 1 WHERE id = '{$_SESSION["AOC_SITE_ID"]}'";
						$conn->Execute($sql);
					}else{
						echo "error";
					}
				}else{
					$template->assign_block_vars("ERROR_FORM",array(
																														"Display"	=>	$lang->_module->FormMsg($validar_campos)
																													)
																			);
					if (preg_match("/password/i", $validar_campos)) {
						$template->assign_var("namev", " value=\"{$Login['name']}\"");
					}
				}
			}
	
			$template->assign_vars(array(
																		"FORM_NAMEPJ"				=>	$lang->_module->FormText('namepj'),
																		"FORM_ORIGEN"				=>	$lang->_module->FormText('origen'),
																		"FORM_RAZA"					=>	$lang->_module->FormText('raza'),
																		"FORM_CLASE"				=>	$lang->_module->FormText('clase'),
																		"FORM_GENERO"				=>	$lang->_module->FormText('genero'),
																		"FORM_ATRIBUTOS"		=>	$lang->_module->FormText('atributos'),
																		"FORM_FUERZA"				=>	$lang->_module->FormText('fuerza'),
																		"FORM_AGILIDAD"			=>	$lang->_module->FormText('agilidad'),
																		"FORM_INTELIGENCIA"	=>	$lang->_module->FormText('inteligencia'),
																		"FORM_CARISMA"			=>	$lang->_module->FormText('carisma'),
																		"FORM_CONSTITUCION"	=>	$lang->_module->FormText('constitucion'),
																		"FORM_SUBMIT"				=>	$lang->_module->FormText('submit')
																	)
														);
			if($row = $conn->Execute("SELECT * FROM acc_cities")){
				while(!$row->EOF()){
					$template->assign_block_vars("CITIES_SHOW",array(
																														"ID"		=>	$row->id,
																														"CITY"	=>	$row->city
																													)
																			);
					$row->MoveNext();
				}
			}
			if($row = $conn->Execute("SELECT * FROM acc_classes")){
				while(!$row->EOF()){
					$template->assign_block_vars("CLASSES_SHOW",array(
																														"ID"		=>	$row->id,
																														"CLASS"	=>	$row->class
																													)
																			);
					$row->MoveNext();
				}
			}
			$init_session = true;
			if($row = $conn->Execute("SELECT * FROM acc_races")){
				while(!$row->EOF()){
					$template->assign_block_vars("RACES_SHOW",array(
																														"ID"		=>	$row->id,
																														"RACE"	=>	$row->race
																													)
																			);
					if($init_session){
						$_SESSION["add_strength"]			=	$row->add_strength;
						$_SESSION["add_agility"]			=	$row->add_agility;
						$_SESSION["add_inteligence"]	=	$row->add_inteligence;
						$_SESSION["add_charisma"]			=	$row->add_charisma;
						$_SESSION["add_constitution"]	=	$row->add_constitution;
						$template->assign_vars(array(
																						"ADD_STRENGTH"			=>	$_SESSION["add_strength"],
																						"ADD_AGILITY"				=>	$_SESSION["add_agility"],
																						"ADD_INTELIGENCE"		=>	$_SESSION["add_inteligence"],
																						"ADD_CHARISMA"			=>	$_SESSION["add_charisma"],
																						"ADD_CONSTITUTION"	=>	$_SESSION["add_constitution"]
																				)
																	);
						$init_session = false;
					}
					$row->MoveNext();
				}
			}
			$gender = array(1=>$lang->_module->FormText('genderMasc'),2=>$lang->_module->FormText('genderFem'));
			for($i=1;$i<3;$i++){
				$template->assign_block_vars("GENDER_SHOW",array(
																														"ID"			=>	$i,
																														"GENDER"	=>	$gender[$i]
																													)
																		);
			}
			$_SESSION['DADO_FUERZA']				=	mt_rand(10,18);
			$_SESSION['DADO_AGILIDAD']			=	mt_rand(10,18);
			$_SESSION['DADO_INTELIGENCIA']	=	mt_rand(10,18);
			$_SESSION['DADO_CARISMA']				=	mt_rand(10,18);
			$_SESSION['DADO_CONSTITUCION']	=	mt_rand(10,18);
			$template->assign_vars(array(
																			"FORM_DFUERZA"				=>	$_SESSION['DADO_FUERZA'],
																			"FORM_DAGILIDAD"			=>	$_SESSION['DADO_AGILIDAD'],
																			"FORM_DINTELIGENCIA"	=>	$_SESSION['DADO_INTELIGENCIA'],
																			"FORM_DCARISMA"				=>	$_SESSION['DADO_CARISMA'],
																			"FORM_DCONSTITUCION"	=>	$_SESSION['DADO_CONSTITUCION']
																	)
														);
		break;
	}
	
?>
