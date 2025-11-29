describe('Download System', () => {
  it('downloads to RAM, verifies token, and saves to disk for Wine', () => {

    // 1. SETUP: Define the path where 'e2e-api-test.yml' expects the file
    // Your YML says: ./cypress/downloads/installer.exe
    const DOWNLOAD_PATH = 'cypress/downloads/installer.exe';

    // 2. INTERCEPT & BLOCK BROWSER
    // We stop the browser from downloading so the token isn't burned.
    cy.intercept('GET', '**/download/*', (req) => {
      req.destroy();
    }).as('downloadRequest');

    cy.visit('/');
    cy.contains('.app-card', '7Zip').click();
    cy.get('#installBtn').click();

    // 3. CAPTURE URL
    cy.wait('@downloadRequest').then((interception) => {
      const downloadUrl = interception.request.url;
      const rawToken = downloadUrl.split('/download/')[1].split('?')[0];
      const tokenBuffer = Buffer.from(rawToken, 'utf-8');

      cy.log(`Manual Download from: ${downloadUrl}`);

      // 4. DOWNLOAD VIA REQUEST (Node.js)
      cy.request({
        url: downloadUrl,
        encoding: 'binary', // Keep as raw binary
      }).then((response) => {
        expect(response.status).to.eq(200);

        // --- STEP A: VERIFY IN MEMORY ---
        const fileBuffer = Buffer.from(response.body, 'binary');
        const tailBuffer = fileBuffer.subarray(-64);
        const match = tailBuffer.includes(tokenBuffer);
        expect(match, `Token ${rawToken} must exist in file`).to.be.true;

        // --- STEP B: SAVE TO DISK (The Fix) ---
        // We manually write the binary data to the disk.
        // This creates the physical file that 'wine64' needs later.
        cy.writeFile(DOWNLOAD_PATH, response.body, 'binary').then(() => {
          cy.log(`Successfully saved file to ${DOWNLOAD_PATH}`);
        });
      });
    });
  });
});