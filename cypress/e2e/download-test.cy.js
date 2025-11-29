describe('Download System', () => {
  it('verifies token injection in RAM (handling One-Time Tokens)', () => {

    // 1. INTERCEPT & DESTROY
    // We catch the request, but we call req.destroy()
    // This stops the browser from hitting the server, keeping the token alive.
    cy.intercept('GET', '**/download/*', (req) => {
      req.destroy();
    }).as('downloadRequest');

    cy.visit('/');
    cy.contains('.app-card', '7Zip').click();

    // 2. TRIGGER
    // The browser will try to download, but our interceptor kills it.
    cy.get('#installBtn').click();

    // 3. CAPTURE & MANUALLY DOWNLOAD
    cy.wait('@downloadRequest').then((interception) => {

      const downloadUrl = interception.request.url;

      // Clean the token string for verification
      const rawToken = downloadUrl.split('/download/')[1].split('?')[0];
      const tokenBuffer = Buffer.from(rawToken, 'utf-8');

      cy.log(`Captured URL: ${downloadUrl}`);
      cy.log(`Token expected: ${rawToken}`);

      // 4. DOWNLOAD INTO RAM
      // Now WE use the valid token via cy.request
      cy.request({
        url: downloadUrl,
        encoding: 'binary', // CRITICAL: Get raw bytes
      }).then((response) => {
        expect(response.status).to.eq(200);

        // 5. PROCESS BINARY DATA
        const fileBuffer = Buffer.from(response.body, 'binary');

        // 6. CHECK STRICTLY THE LAST 64 BYTES
        // You said it MUST be in the last 64 bytes.
        const tailBuffer = fileBuffer.subarray(-64);

        // Debugging logs (Check Console if fails)
        console.log('Target Token:', tokenBuffer.toString('hex'));
        console.log('Actual Tail :', tailBuffer.toString('hex'));

        // Check if the token exists within that exact 64-byte window
        const match = tailBuffer.includes(tokenBuffer);

        expect(match, `Token ${rawToken} must exist in the last 64 bytes`).to.be.true;
      });
    });

    cy.log('Test passed: One-time token used successfully in RAM check.');
  });
});