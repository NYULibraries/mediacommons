<?php
use PHPUnit\Framework\TestCase;

final class ApacheConfigurationTest extends TestCase {

    public function testTest() {
        $startUrl = "http://dev.media-commons.org/mediacommons/";
        $expectedEndUrl = "http://dev.mediacommons.org/mediacommons/";
        $expectedNumRedirects = 1;

        list( $effectiveUrl, $numRedirections ) = $this->checkRedirect( $startUrl );

        $this->assertEquals( $expectedEndUrl, $effectiveUrl );
        $this->assertEquals( $expectedNumRedirects, $numRedirections);
    }

    public function checkRedirect( $url ) {
        $ch = curl_init();

        curl_setopt( $ch, CURLOPT_URL, $url );
        curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, TRUE );
        curl_setopt( $ch, CURLOPT_NOBODY, TRUE );

        curl_exec( $ch );

        $effectiveUrl = curl_getinfo( $ch, CURLINFO_EFFECTIVE_URL );
        $redirectCounts = curl_getinfo( $ch, CURLINFO_REDIRECT_COUNT );

        return [ $effectiveUrl, $redirectCounts ];
    }

}
