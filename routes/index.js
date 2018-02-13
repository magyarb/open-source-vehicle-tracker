var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/track/:itemid', function(req, res, next) {
    res.render('index', { title: 'Express' });
});

router.get('/add/:itemid/:itempass', function(req, res, next) {
    res.render('index', { title: 'Express' });
});

router.get('/gps/:lat/:lon/:itemid/:itempass', function(req, res, next) {
    console.log(req.params.lat);
    console.log(req.params.lon);
    res.status(200);
    res.send("OK");
});


module.exports = router;
