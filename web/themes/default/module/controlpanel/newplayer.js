function RealizarSuma(){
	jQuery("td[id='dado']").each(function(){
		var element = jQuery(this);
		var base = parseInt(element.html());
		var sumar = parseInt(jQuery("#"+element.data('final')+"_suma").html());
		var final = jQuery("#"+element.data('final')+"_final");
		final.html(base+sumar);
	});
}
jQuery(document).ready(function(){
	RealizarSuma();
	jQuery("#TirarDados").click(function(){
		jQuery.ajax({
			type: "POST",
			url: "index.php?page=controlpanel&sec=newplayer&act=rollingdice",
			success: function(datos){
				var dados = jQuery.parseJSON(datos);
				jQuery("#dado[data-final='f']").html(dados.df);
				jQuery("#dado[data-final='a']").html(dados.da);
				jQuery("#dado[data-final='i']").html(dados.di);
				jQuery("#dado[data-final='ca']").html(dados.dca);
				jQuery("#dado[data-final='co']").html(dados.dco);
				RealizarSuma();
			}
		});
	});
	jQuery("select[name='NewPJ[race]']").change(function(){
		var race = jQuery(this).val();
		jQuery.ajax({
			type: "POST",
			url: "index.php?page=controlpanel&sec=newplayer&act=changerace",
			data: "race="+encodeURIComponent(race),
			success: function(datos){
				var dados = jQuery.parseJSON(datos);
				jQuery("#f_suma").html('+'+dados.df);
				jQuery("#a_suma").html('+'+dados.da);
				jQuery("#i_suma").html('+'+dados.di);
				jQuery("#ca_suma").html('+'+dados.dca);
				jQuery("#co_suma").html('+'+dados.dco);
				RealizarSuma();
			}
		});
	});
});
