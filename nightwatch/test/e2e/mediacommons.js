var conf = require('../../nightwatch.conf.js');

module.exports = {

  beforeEach: function(client, done) {
    client.url('http://mediacommons.local/users/ana-cabral-martins', function() {
      done();
    });
  },

  'MC-442 - Can visit clean URLs' : function (browser) {
    browser
      .assert.urlEquals("http://mediacommons.local/users/ana-cabral-martins")
      .assert.containsText('h1.p-name.fn', 'Ana Cabral Martins')
      .end();
  },

  'Finished': function(client) {
    client
      .perform(() => {
        console.log('[perform]: Finished Test:', client.currentTest.name)
      })
      .end();
  }

  
    



      

    // part two:
    //browser
    //  .assert.containsText('body', 'dwyl.com') // assert body contains text
    //  .saveScreenshot(conf.imgpath(browser) + 'dwyl.png')
    //  .end();

   //  }
  };
