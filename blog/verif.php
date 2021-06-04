<?php
session_start() ;
if(!isset($_SESSION['pseudo']))
{
  header('Location:../Conn_Insc/connexion.php');
}
?>
