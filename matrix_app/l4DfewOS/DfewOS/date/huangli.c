
var directions = ["北方","东北方","东方","东南方","南方","西南方","西方","西北方"];
var activities = [
	{name:"写单元测试", good:"写单元测试将减少出错",bad:""},
	{name:"洗澡", good:"你几天没洗澡了？",bad:"会把设计方面的灵感洗掉"},
	{name:"捡肥皂", good:"安全无事",bad:"你懂的"},
	{name:"约会/相亲", good:"很容易获得好感",bad:""},
	{name:"理发", good:"你几个月没理发了？",bad:""},
	{name:"抽烟", good:"抽烟有利于提神，增加思维敏捷",bad:"导致思路混乱，程序写不出"},
	{name:"锻炼一下身体", good:"有助于减肥",bad:"能量没消耗多少，吃得却更多"},
	{name:"代码重构", good:"代码质量得到提高",bad:"你很有可能会陷入泥潭"},
	{name:"使用%v1", good:"你看起来更有品位",bad:"别人会觉得你在装逼"},
	{name:"招人", good:"你遇到千里马的可能性大大增加",bad:"你只会招到混饭吃的外行"},
	{name:"申请加薪", good:"老板今天心情很好",bad:""},
	{name:"晚上加班", good:"晚上是程序员精神最好的时候",bad:"事倍功半"},
	{name:"在妹子面前吹牛", good:"改善你矮穷挫的形象",bad:"会被识破"},
	{name:"提交代码", good:"遇到冲突的几率是最低的",bad:"遇到代码冲突"},
	{name:"代码复审", good:"可以发现重要问题",bad:"什么问题都发现不了"},
	{name:"开会", good:"写代码之余放松一下打个盹，有益健康",bad:"你会被扣屎盆子背黑锅"},
	{name:"下班玩%v3", good:"你将有如神助",bad:"你会被虐的很惨"},
	{name:"修复BUG", good:"你今天对BUG的嗅觉大大提高",bad:"新产生的BUG将比修复的更多"},
	{name:"设计评审", good:"设计评审会议将变成头脑风暴",bad:"人人筋疲力尽，评审就这么过了"},
	{name:"需求变更", good:"",bad:"还让不让人活了？"},
	{name:"上微博", good:"今天发生的事不能错过",bad:"会被上司看到"},
	{name:"打球", good:"劳逸结合",bad:"小心受伤"},
	{name:"买彩票", good:"很有可能有不错的收获",bad:"把钱扔水里"},
	{name:"请假", good:"得到更好的休息",bad:""},
	{name:"请销售mm喝茶", good:"也许会有不错的收获",bad:"被当成色狼"},
	{name:"郊游", good:"放松心情，亲近大自然",bad:""},
	{name:"上班%v4", good:"还需要理由吗？",bad:"会被上司看到"}
];

var tools = ["记事本写程序", "Windows8", "Linux", "MacOS", "QQ浏览器", "三星手机", "iphone"];

var food = ["麦当劳", "肯德基", "老盛昌", "盒饭", "四海游龙", "自己带的饭", "永和大王", "生煎锅贴", "批萨","甜点","肉包子","火锅","蛋包饭","牛排","鸡公煲","村夫烤鱼","麻辣烫","桂林米粉","鸿盛兴","咖喱饭"];

var drinks = ["水","红茶","绿茶","咖啡","奶茶","卡布基诺","牛奶","豆奶","果汁","果味汽水","可乐","运动饮料","酸奶","酒"];

var clothing_color = ["花色","红色", "橙色", "黄色", "绿色", "青色", "蓝色", "紫色", "黑色", "白色", "灰色", "褐色", "豹纹"];

var clothing = ["外套", "衬衫", "内衣", "T恤", "赤膊"];

var clothing2 = ["内裤", "裤子", "鞋子", "袜子", "帽子"];

var game = ["Dota","暗黑3","魔兽世界","CS","魔兽争霸","植物大战僵尸","愤怒的小鸟"];

var yang = ["放羊","玩游戏","看电影","看无聊网站","QQ和MM聊天","看猥琐图"];

var vehicle = ["坐出租车","骑自行车","开汽车","坐地铁","骑摩托车","开电瓶车","步行"];

var place = ["陆家嘴","公园","公司","购物","探亲","拜访好友","约会"];

var who = ["黄晶晶", "陈刚", "刘姣姣", "陈云云", "余剑"];

var where = ["外滩","公司","会议室","厕所","夏威夷","珠穆朗玛峰","家里","麦当劳","沙滩"];

var dosth = ["跳肚皮舞","看美女","搓麻将","炸金花","玩DOTA","写代码","洗桑拿","听神曲忐忑","打实况足球","嗑瓜子","劫色","吹牛逼","看国足比赛"];

var star = ["蜘蛛侠","绿巨人","凤姐","喜羊羊","超人","芙蓉姐姐","肯德基老爷爷","老版","范冰冰","梅西"];







// 从 activities 中随机挑选 size 个
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

// 解析占位符并替换成随机内容
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
 * 注意：本程序中的“随机”都是伪随机概念，以当前的天为种子。
 */
function random(dayseed, indexseed) {
	var n = dayseed % 11117;
	var r;
	for (var i = 0; i < 100 + indexseed; i++) {
		n = n * n + 11117;
		n = n % 11117;   // 11117 是个质数
	}
	return n;
}

$(function(){
	$('.date').html(getTodayString());
	$('.direction_value').html(directions[random(iday, 2) % directions.length]);
	$('.drink_value').html(drinks[random(iday, 5) % drinks.length]);
	$('.eat_value').html(food[random(iday, 3) % food.length]);
	var clothing1 = clothing[random(iday, 11) % clothing.length];
	if(clothing1 != '赤膊')
		$('.clothingColor_value').html(clothing_color[random(iday, 13) % clothing_color.length]);
	$('.clothing_value').html(clothing1);
	$('.clothingColor_value2').html(clothing_color[random(iday, 17) % clothing_color.length]);
	$('.clothing_value2').html(clothing2[random(iday, 19) % clothing2.length]);
	$('.vehicle').html(vehicle[random(iday, 21) % vehicle.length]);
	$('.place').html(place[random(iday, 23) % place.length]);
	$('.event').html(who[random(iday, 25) % who.length] + "<font style='color:black'>和</font>" +star[random(iday, 26) % star.length] + "<font style='color:black'>在</font>" + 
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
