var mouse;
var me;
var stage;
Stage.FLUSH_INTERVAL = 30;
Stage.BACK_GROUND = new Image();
Stage.BACK_GROUND.src = 'image/bg2.jpg';
Stage.WIDTH = 900;
Stage.HEIGHT = 540;  

//倒计时模块
var intDiff = parseInt(79);
function timer(intDiff){
	window.setInterval(function(){
	var second=0;//时间默认值
	if(intDiff > 0)
		second = Math.floor(intDiff);
		$('#second_show').html('<s></s>'+second+'s');
		intDiff--;
		}, 1000);
} 
$(function(){timer(intDiff);});	

//捕捉鼠标位置
function captureMouse(element) {
	var mouse={x:900/2, y:540/2};
	element.addEventListener('mousemove',function(event){
	var x,y;
	if ( event.pageX || event.pageY ) {
		x = event.pageX;
		y = event.pageY;
	} else {
		x = event.clientX + document.body.scrollLeft
			+ document.documentElement.scrollLeft;
		y = event.clientY + document.body.scrollTop
			+ document.documentElement.scrollTop;
	}
	x -= element.offsetLeft;
	y -= element.offsetTop;
	mouse.x = x;
	mouse.y = y;
	}, false);
	return mouse;
}
	
function Stage(canvasId) {
	this.objects = [];
	this.running = false;
	this.ballCurrentInterval = Balloon.CURRENT_INTERVAL;
	canvasId = canvasId || 'canvas';
	this.ctx = document.getElementById(canvasId).getContext('2d');
	this.ctx.fillStyle    = '#C71C60';
	this.ctx.font         = 'bold 32px Cursive';
	this.ctx.textBaseline = 'top';
}

Stage.prototype = {
    start : function() {
		this.running = true;
		this.lastGenBallTime = new Date().getTime();
		this.lastSpeedUpTime = new Date().getTime();
		Stage.current = this;
		Stage.current.speedUp = 0;
		Stage.current.totalScore = 0;
		this.recordDefShadow();

		var xIndex = parseInt(Stage.WIDTH / Balloon.WIDTH);
		this.availableX = [];
		for (var i = 0; i < xIndex; i++) {
			this.availableX[i] = Balloon.WIDTH * i;
		}
		
		this.run();
		
		$("#canvasDiv").click(function(e){ 
			var offset = $("#canvasDiv").offset();
			var x = e.pageX - offset.left;
			var y = e.pageY - offset.top;
			var oldSpeedUp = Stage.current.speedUp;
			for (var i in Stage.current.objects) {
				var obj = Stage.current.objects[i];
				if (x > obj.x && x < obj.x + Balloon.WIDTH
					&& y > obj.y && y < obj.y + Balloon.HEIGHT) {

					clickOn = true;
					obj.hide = true;
					$('#misc')[0].play();
					if ('GFIT_BALL_1' == obj.color || 'GFIT_BALL_2' == obj.color) {
						Stage.current.speedUp = 0;
					}
					Stage.current.totalScore += obj.score;
				}
			}
			
			if (oldSpeedUp != Stage.current.speedUp) {
				for (var i in Stage.current.objects) {
					if (Stage.current.objects[i].speed > oldSpeedUp) {
						Stage.current.objects[i].speed -= oldSpeedUp;
					}
				}
			}			
		});
    },

	recordDefShadow: function() {
		this.defShadow = {};
		this.defShadow.fillStyle = this.ctx.fillStyle;
		this.defShadow.font = this.ctx.font;
		this.defShadow.shadowOffsetX = this.ctx.shadowOffsetX;
		this.defShadow.shadowOffsetY = this.ctx.shadowOffsetY;
		this.defShadow.shadowBlur = this.ctx.shadowBlur;
		this.defShadow.shadowColor = this.ctx.shadowColor;
		this.defShadow.textBaseline = this.ctx.textBaseline;
	},

	drawScore: function() {
		this.ctx.fillText('SCORE:' + Stage.current.totalScore , 0, 0);
	},

	run : function() {
		// 每隔一段时间添加新的气球
		var currentTime = new Date().getTime();
		var diff = currentTime - this.lastGenBallTime;
		if (diff >= Balloon.GEN_INTERVAL) {
			this.lastGenBallTime = currentTime;

			if (!this.ballCurrentInterval) {
				this.ballCurrentInterval = Balloon.CURRENT_INTERVAL;
			}
			
			for (var i = 0; i < this.ballCurrentInterval; i++) {
				var index = parseInt(Math.random() * Balloon.COLOR.length);
				var ball = new Balloon();
				ball.x = this.availableX[parseInt(Math.random() * this.availableX.length)];
				ball.y = Stage.HEIGHT - Balloon.HEIGHT;
				ball.color = Balloon.COLOR[index];
				ball.score = Balloon.SCORE[ball.color];
				ball.image = Balloon.IMAGE[index];
				ball.speed = Balloon.SPEED[ball.color];
				ball.width = Balloon.WIDTH;
				ball.height = Balloon.WIDTH;
				if (Stage.current.speedUp) {
					ball.speed += Stage.current.speedUp;
				}
				Stage.current.objects.push(ball);
			}
			this.ballCurrentInterval--;
			
		}
		
		// 每隔一段时间加速
		diff = currentTime - this.lastSpeedUpTime;
		if (diff >= Balloon.SPEED_UP_INTERVAL) {
			this.lastSpeedUpTime = currentTime;
			Stage.current.speedUp += Balloon.SPEED_INC;
		}
		
		for (var i in Stage.current.objects) {
			var obj = Stage.current.objects[i];
			obj.fly();
		}

		var newObjects = [];
		
		this.ctx.clearRect(0, 0, Stage.WIDTH, Stage.HEIGHT);
		this.ctx.save();
		this.ctx.drawImage(Stage.BACK_GROUND, 0, 0);
		
		for (var i in Stage.current.objects) {
			var obj = Stage.current.objects[i];
			if (!obj.hide) {
				newObjects.push(obj);
				this.ctx.drawImage(obj.image, obj.x, obj.y);
				} 
				this.ctx.fillText(obj.score, obj.x + 78, obj.y + 55);	//显示分数
		}
		this.ctx.drawImage(me, mouse.x, mouse.y - 120);  //鼠标
		this.ctx.restore();  
		Stage.current.objects = null;
		Stage.current.objects = newObjects;
		
		this.drawScore();

		if (this.running) {
			var _this = this;
			setTimeout(function() {
				_this.run();
			}, Stage.FLUSH_INTERVAL);
		}
	},

	stop : function() {
		if (this.running) {
			this.running = false;
		}
	}
}

function GameObject() {
    this.image = null;
    this.x = 0;
    this.y = 0;
    this.width = 0;
    this.height = 0;

    this.speed = 1;
    this.hide = false;
}

function Balloon() {
    this.score = null;
	this.color = null;
}

Balloon.MOVE_DIST = 1;
Balloon.GEN_INTERVAL = 1000;
Balloon.SPEED_UP_INTERVAL = 2500;
Balloon.SPEED_INC = 0.5;
Balloon.COLOR = ['GIFT_1', 'GIFT_2', 'BLUE', 'GREEN', 'ORANGE', 'RED', 'BOMB'];
Balloon.SPEED = { 'GIFT_1':4, 'GIFT_2':4, 'BLUE':3, 'GREEN':3, 'ORANGE':2, 'RED':1, 'BOMB':2 };
Balloon.SCORE = { 'GIFT_1':0, 'GIFT_2':4, 'BLUE':3, 'GREEN':3, 'ORANGE':2, 'RED':1, 'BOMB':-6 };

var GFIT_BALL_1 = new Image(); GFIT_BALL_1.src = 'image/ball_gift1.png';
var GFIT_BALL_2 = new Image(); GFIT_BALL_2.src = 'image/ball_gift2.png';
var GREEN_BALL = new Image(); GREEN_BALL.src = 'image/green.png';
var BLUE_BALL = new Image(); BLUE_BALL.src = 'image/blue.png';
var ORANGE_BALL = new Image(); ORANGE_BALL.src = 'image/orange.png';
var RED_BALL = new Image(); RED_BALL.src = 'image/red.png';
var BOMB_BALL = new Image(); BOMB_BALL.src = 'image/ball_bomb.png';
Balloon.IMAGE = [GFIT_BALL_1, GFIT_BALL_2, GREEN_BALL, BLUE_BALL, ORANGE_BALL, RED_BALL, BOMB_BALL];
Balloon.WIDTH = 120;
Balloon.HEIGHT = 120;
Balloon.MAX_SPEED = 5;
Balloon.CURRENT_INTERVAL = 3;

Balloon.prototype = new GameObject();
Balloon.prototype.fly = function() {
    this.y -= (this.speed * Balloon.MOVE_DIST);
    if (!this.hide && this.y < 0) {
        this.hide = true;
    }
}

function init() { 
	mouse = captureMouse(document.getElementById('canvas'));
	me = new Image();
	me.src = "image/dart.png";
	
	stage = new Stage();
	stage.start();
}

function gameOver() {
	stage.stop();
	alert('score :' + Stage.current.totalScore);	
}

setTimeout(gameOver , 81 * 1000);