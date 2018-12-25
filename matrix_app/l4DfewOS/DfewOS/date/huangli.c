
var directions = ["����","������","����","���Ϸ�","�Ϸ�","���Ϸ�","����","������"];
var activities = [
	{name:"д��Ԫ����", good:"д��Ԫ���Խ����ٳ���",bad:""},
	{name:"ϴ��", good:"�㼸��ûϴ���ˣ�",bad:"�����Ʒ�������ϴ��"},
	{name:"�����", good:"��ȫ����",bad:"�㶮��"},
	{name:"Լ��/����", good:"�����׻�úø�",bad:""},
	{name:"��", good:"�㼸����û���ˣ�",bad:""},
	{name:"����", good:"������������������˼ά����",bad:"����˼·���ң�����д����"},
	{name:"����һ������", good:"�����ڼ���",bad:"����û���Ķ��٣��Ե�ȴ����"},
	{name:"�����ع�", good:"���������õ����",bad:"����п��ܻ�������̶"},
	{name:"ʹ��%v1", good:"�㿴��������Ʒλ",bad:"���˻��������װ��"},
	{name:"����", good:"������ǧ����Ŀ����Դ������",bad:"��ֻ���е��췹�Ե�����"},
	{name:"�����н", good:"�ϰ��������ܺ�",bad:""},
	{name:"���ϼӰ�", good:"�����ǳ���Ա������õ�ʱ��",bad:"�±�����"},
	{name:"��������ǰ��ţ", good:"�����㰫��������",bad:"�ᱻʶ��"},
	{name:"�ύ����", good:"������ͻ�ļ�������͵�",bad:"���������ͻ"},
	{name:"���븴��", good:"���Է�����Ҫ����",bad:"ʲô���ⶼ���ֲ���"},
	{name:"����", good:"д����֮�����һ�´������潡��",bad:"��ᱻ��ʺ���ӱ��ڹ�"},
	{name:"�°���%v3", good:"�㽫��������",bad:"��ᱻŰ�ĺܲ�"},
	{name:"�޸�BUG", good:"������BUG�����������",bad:"�²�����BUG�����޸��ĸ���"},
	{name:"�������", good:"���������齫���ͷ�Է籩",bad:"���˽�ƣ�������������ô����"},
	{name:"������", good:"",bad:"���ò����˻��ˣ�"},
	{name:"��΢��", good:"���췢�����²��ܴ��",bad:"�ᱻ��˾����"},
	{name:"����", good:"���ݽ��",bad:"С������"},
	{name:"���Ʊ", good:"���п����в�����ջ�",bad:"��Ǯ��ˮ��"},
	{name:"���", good:"�õ����õ���Ϣ",bad:""},
	{name:"������mm�Ȳ�", good:"Ҳ����в�����ջ�",bad:"������ɫ��"},
	{name:"����", good:"�������飬�׽�����Ȼ",bad:""},
	{name:"�ϰ�%v4", good:"����Ҫ������",bad:"�ᱻ��˾����"}
];

var tools = ["���±�д����", "Windows8", "Linux", "MacOS", "QQ�����", "�����ֻ�", "iphone"];

var food = ["����", "�ϵ»�", "��ʢ��", "�з�", "�ĺ�����", "�Լ����ķ�", "���ʹ���", "�������", "����","���","�����","���","������","ţ��","������","�����","������","�����׷�","��ʢ��","��ଷ�"];

var drinks = ["ˮ","���","�̲�","����","�̲�","������ŵ","ţ��","����","��֭","��ζ��ˮ","����","�˶�����","����","��"];

var clothing_color = ["��ɫ","��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "��ɫ", "����"];

var clothing = ["����", "����", "����", "T��", "�ಲ"];

var clothing2 = ["�ڿ�", "����", "Ь��", "����", "ñ��"];

var game = ["Dota","����3","ħ������","CS","ħ������","ֲ���ս��ʬ","��ŭ��С��"];

var yang = ["����","����Ϸ","����Ӱ","��������վ","QQ��MM����","�����ͼ"];

var vehicle = ["�����⳵","�����г�","������","������","��Ħ�г�","����ƿ��","����"];

var place = ["½����","��԰","��˾","����","̽��","�ݷú���","Լ��"];

var who = ["�ƾ���", "�¸�", "����", "������", "�ལ"];

var where = ["��̲","��˾","������","����","������","���������","����","����","ɳ̲"];

var dosth = ["����Ƥ��","����Ů","���齫","ը��","��DOTA","д����","ϴɣ��","����������","��ʵ������","ྐྵ���","��ɫ","��ţ��","���������"];

var star = ["֩����","�̾���","���","ϲ����","����","ܽ�ؽ��","�ϵ»���үү","�ϰ�","������","÷��"];







// �� activities �������ѡ size ��
function pickRandom(size) {
	var result = [];
	
	for (var i = 0; i < activities.length; i++) {
		result.push(parse(activities[i]));
	}
	
	for (var j = 0; j < activities.length - size; j++) {
		var index = random(iday, j) % result.length;
		result.splice(index, 1);
	}
	
	return result;
}

// ����ռλ�����滻���������
function parse(event) {
	var result = {name: event.name, good: event.good, bad: event.bad};  // clone
	
	if (result.name.indexOf('%v1') != -1) {
		result.name = result.name.replace('%v1', tools[random(iday, 3) % tools.length]);
	}
	
	if (result.name.indexOf('%v2') != -1) {
		result.name = result.name.replace('%v2', food[random(iday, 5) % food.length]);
	}
	
	if (result.name.indexOf('%v3') != -1) {
		result.name = result.name.replace('%v3', game[random(iday, 7) % game.length]);
	}
	
	if (result.name.indexOf('%v4') != -1) {
		result.name = result.name.replace('%v4', yang[random(iday, 9) % yang.length]);
	}
	
	return result;
}



/*
 * ע�⣺�������еġ����������α�������Ե�ǰ����Ϊ���ӡ�
 */
function random(dayseed, indexseed) {
	var n = dayseed % 11117;
	var r;
	for (var i = 0; i < 100 + indexseed; i++) {
		n = n * n + 11117;
		n = n % 11117;   // 11117 �Ǹ�����
	}
	return n;
}

$(function(){
	$('.date').html(getTodayString());
	$('.direction_value').html(directions[random(iday, 2) % directions.length]);
	$('.drink_value').html(drinks[random(iday, 5) % drinks.length]);
	$('.eat_value').html(food[random(iday, 3) % food.length]);
	var clothing1 = clothing[random(iday, 11) % clothing.length];
	if(clothing1 != '�ಲ')
		$('.clothingColor_value').html(clothing_color[random(iday, 13) % clothing_color.length]);
	$('.clothing_value').html(clothing1);
	$('.clothingColor_value2').html(clothing_color[random(iday, 17) % clothing_color.length]);
	$('.clothing_value2').html(clothing2[random(iday, 19) % clothing2.length]);
	$('.vehicle').html(vehicle[random(iday, 21) % vehicle.length]);
	$('.place').html(place[random(iday, 23) % place.length]);
	$('.event').html(who[random(iday, 25) % who.length] + "<font style='color:black'>��</font>" +star[random(iday, 26) % star.length] + "<font style='color:black'>��</font>" + 
        where[random(iday, 31) % where.length] + dosth[random(iday, 33) % dosth.length]);
	var starHtml = "";
	for(var i = 0; i < 5; i++)
	{
		var _star = who[random(iday, i+32) % who.length];
		if(starHtml.indexOf(_star) == -1)
		{
			starHtml += "<span>"+_star+"<span>?"
		}
	}
	$('.star').html(starHtml);

    var numGood = 2;
	var numBad = 2;
	var eventArr = pickRandom(numGood + numBad);
	
	
	for (var i = 0; i < numGood; i++) {
		addToGood(eventArr[i]);
	}
	
	for (var i = 0; i < numBad; i++) {
		addToBad(eventArr[numGood + i]);
	}
});


#include "huangli.h"
int huangli_get_info (int year, int month, int day, HUANGLI_INFO *pHuangli)
{
    return (0);
}
