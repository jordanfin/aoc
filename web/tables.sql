-- phpMyAdmin SQL Dump
-- version 3.3.7deb5build0.10.10.1
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Tiempo de generación: 02-08-2011 a las 13:28:47
-- Versión del servidor: 5.1.49
-- Versión de PHP: 5.3.3-1ubuntu9.5

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de datos: `aoc_dev`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `account`
--

CREATE TABLE IF NOT EXISTS `account` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(25) NOT NULL,
  `password` varchar(50) NOT NULL,
  `email` varchar(255) NOT NULL,
  `players` int(11) NOT NULL,
  `ip` varchar(25) NOT NULL,
  `ban` tinyint(1) NOT NULL,
  `online` tinyint(1) NOT NULL,
  `active` tinyint(1) NOT NULL,
  `codeactive` varchar(100) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

--
-- Volcar la base de datos para la tabla `account`
--


-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `acc_cities`
--

CREATE TABLE IF NOT EXISTS `acc_cities` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `city` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

--
-- Volcar la base de datos para la tabla `acc_cities`
--

INSERT INTO `acc_cities` (`id`, `city`) VALUES
(1, 'Alguna');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `acc_classes`
--

CREATE TABLE IF NOT EXISTS `acc_classes` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `class` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

--
-- Volcar la base de datos para la tabla `acc_classes`
--

INSERT INTO `acc_classes` (`id`, `class`) VALUES
(1, 'Saltarin');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `acc_players`
--

CREATE TABLE IF NOT EXISTS `acc_players` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `account_id` int(11) NOT NULL,
  `class` int(11) NOT NULL,
  `gender` tinyint(1) NOT NULL,
  `race` int(11) NOT NULL,
  `home` int(11) NOT NULL,
  `head` int(11) NOT NULL,
  `body` int(11) NOT NULL,
  `strength` int(11) NOT NULL,
  `agility` int(11) NOT NULL,
  `inteligence` int(11) NOT NULL,
  `charisma` int(11) NOT NULL,
  `constitution` int(11) NOT NULL,
  `level` int(11) NOT NULL DEFAULT '1',
  `exp` int(11) NOT NULL,
  `gold` int(11) NOT NULL,
  `weapon` int(11) NOT NULL,
  `shield` int(11) NOT NULL,
  `helmet` int(11) NOT NULL,
  `maxhp` int(11) NOT NULL,
  `minhp` int(11) NOT NULL,
  `maxsta` int(11) NOT NULL,
  `minsta` int(11) NOT NULL,
  `maxmn` int(11) NOT NULL,
  `minmn` int(11) NOT NULL,
  `maxhit` int(11) NOT NULL,
  `mithit` int(11) NOT NULL,
  `maxham` int(11) NOT NULL,
  `minham` int(11) NOT NULL,
  `maxagu` int(11) NOT NULL,
  `minagu` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_acc_races_id_acc_players_race` (`race`),
  KEY `fk_acc_classes_id_acc_players_class` (`class`),
  KEY `fk_account_id_acc_players_account_id` (`account_id`),
  KEY `fk_acc_cities_id_acc_players_home` (`home`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

--
-- Volcar la base de datos para la tabla `acc_players`
--


-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `acc_races`
--

CREATE TABLE IF NOT EXISTS `acc_races` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `race` varchar(20) NOT NULL,
  `add_strength` int(11) NOT NULL,
  `add_agility` int(11) NOT NULL,
  `add_inteligence` int(11) NOT NULL,
  `add_charisma` int(11) NOT NULL,
  `add_constitution` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3 ;

--
-- Volcar la base de datos para la tabla `acc_races`
--

INSERT INTO `acc_races` (`id`, `race`, `add_strength`, `add_agility`, `add_inteligence`, `add_charisma`, `add_constitution`) VALUES
(1, 'Humano', 1, 1, 0, 0, 2),
(2, 'Elfo', 1, 2, 1, 0, 1);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `config`
--

CREATE TABLE IF NOT EXISTS `config` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `config` varchar(50) NOT NULL,
  `value` varchar(255) NOT NULL,
  `titleconfig` varchar(100) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;

--
-- Volcar la base de datos para la tabla `config`
--

INSERT INTO `config` (`id`, `config`, `value`, `titleconfig`) VALUES
(1, 'copyright', 'Parra Studios Copyright Â© 2009 - 2012', 'Copyright'),
(2, 'title_site', 'Argentum Online C Developing', 'TÃ­tulo del sitio'),
(3, 'theme', 'default', 'Theme'),
(4, 'maxplayers', '5', 'Maxima cant. Personajes');

--
-- Filtros para las tablas descargadas (dump)
--

--
-- Filtros para la tabla `acc_players`
--
ALTER TABLE `acc_players`
  ADD CONSTRAINT `acc_players_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `account` (`id`) ON DELETE CASCADE ON UPDATE NO ACTION,
  ADD CONSTRAINT `acc_players_ibfk_2` FOREIGN KEY (`class`) REFERENCES `acc_classes` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `acc_players_ibfk_3` FOREIGN KEY (`race`) REFERENCES `acc_races` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `acc_players_ibfk_4` FOREIGN KEY (`home`) REFERENCES `acc_cities` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;
