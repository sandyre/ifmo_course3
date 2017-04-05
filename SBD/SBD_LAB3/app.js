var mongoose = require('mongoose');
require('mongoose-double')(mongoose);
mongoose.connect('mongodb://localhost:27017/SBD_L3');
var db = mongoose.connection;

var importer_scheme = new mongoose.Schema(
{
	name : {
		type : String,
		required : true,
		minlength : 3,
		maxlength : 20
	},
	phone : {
		type : String,
		required : true,
		minlength : 10,
		maxlength : 10,
		match : [ /\d{10}$/, "doesnt match"]
	},
	email : {
		type : String,
		required : true,
		minlength : 5,
		maxlength : 30
	}
});

var import_scheme = new mongoose.Schema(
{
	importer_id : {
		type : mongoose.Schema.Types.ObjectId,
		required : true
	},
	product_id : {
		type : mongoose.Schema.Types.ObjectId,
		required : true
	},
	amount : {
		type : mongoose.Schema.Types.Double,
		min : 0
	},
	price : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	},
	sum : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	},
	date : {
		type : Date,
		required : true	
	}
});

var product_scheme = new mongoose.Schema(
{
	name : {
		type : String,
		required : true,
		minlength : 3,
		maxlength : 30
	},
	amount : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	},
	price : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	}
});

var menu_food_scheme = new mongoose.Schema(
{
	name : {
		type : String,
		required : true,
		minlength : 3,
		maxlength : 30
	},
	price : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	},
	type : {
		type : String,
		required : true,
		minlength : 3,
		maxlength : 30
	},
	measure : {
		type : String,
		required : true,
		minlength : 2,
		maxlength : 5
	},
	out : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	},
	view : {
		type : mongoose.Schema.Types.Buffer,
		required : false
	}
});

var reciepe_menu_scheme = new mongoose.Schema(
{
	menu_food_id : {
		type : mongoose.Schema.Types.ObjectId,
		required : true
	},
	product_id : {
		type : mongoose.Schema.Types.ObjectId,
		required : true
	},
	amount : {
		type : mongoose.Schema.Types.Double,
		required : true,
		min : 0
	}
});

db.on('error', function(err) {
	console.log('connection error', err);
});

db.once('open', function() {
	var Importer = mongoose.model('Importer', importer_scheme);
	var test = new Importer({
		name : 'Kekus',
		phone : '8005553535',
		email : 'zaym@denga.ru'
	});
	
	test.save(function(err, data) {
		if(err) console.log(err);
		else console.log('Saved : ', data);
	});

	var Product = mongoose.model('Product', product_scheme);
	test = new Product({
		name : 'Morkovka',
		amount : 10.0,
		price : 50.0
	});

	test.save(function(err, data) {
		if(err) console.log(err);
		else console.log('Saved : ', data);
	});

	var Import = mongoose.model('Import', import_scheme);
	test = new Import({
		importer_id : mongoose.Types.ObjectId('5845ae6cb3951039d008d110'),
		product_id : mongoose.Types.ObjectId('5846b7e71cb7110a0acd2c2d'),
		amount : 10.5,
		price : 30.5,
		sum : 325.0,
		date : new Date('31 December, 2020')
	});

	test.save(function(err, data) {
		if(err) console.log(err);
		else console.log('Saved : ', data);
	});
});
