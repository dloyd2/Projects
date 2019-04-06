using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class PlayerStats : Entity
{
    [SerializeField] protected float maxBreath = 6;
    private float currentBreath;
    private float flashTimer = 0.0f;
    private float flashTime = 0.75f;
    private float flashSpeed = 1.5f;
    Color flashColor = new Color(1f, 0f, 0f, 0.3f);
    public PlayerMovement playerMovement;
    public Image damageImage;
    public Image deathImage;
    public Image healthSlider;
    
    [SerializeField]protected Image defenseShield1;
    [SerializeField]protected Image defenseShield2;
    [SerializeField]protected Image defenseShield3;
    [SerializeField]protected Image defenseShield4;
    protected Color tempDefColor = new Color(1, 1, 1, 0);
    
    protected override void Death()
    {
        isDead = true;
        playerMovement.horizontalMove = 0;
        playerMovement.enabled = false;
        deathImage.enabled = true;
    }
    
    void Awake()
    {
        deathImage.enabled = false;
        ChangeDefense(0);
        currentBreath = maxBreath;
    }
    void updateHealthUI()
    {
        healthSlider.fillAmount = remainingHealth/maxHealth;        
    }
    protected override void HandleDamage()
    {
        remainingHealth -= damageTaken;
        damageImage.color = flashColor;
        flashTimer += Time.deltaTime;
    }
    
    // Update is called once per frame
    protected override void UpdateStatus()
    {
        if(flashTimer != 0)
        {
            flashTimer = flashTimer <= flashTime ? flashTimer+Time.deltaTime : 0.0f;
            damageImage.color = Color.Lerp(damageImage.color, Color.clear, flashSpeed*Time.deltaTime);
        }
        updateHealthUI();
    }
    
    public void ChangeDefense(int toAdd)
    {
        int result = currentDefense+toAdd;
        currentDefense = result < maxDefense ? result : maxDefense;
        switch(currentDefense)
        {
            case 1: tempDefColor.a = 1;
                defenseShield1.color = tempDefColor;
                break;
            case 2: tempDefColor.a = 1;
                defenseShield1.color = tempDefColor;
                defenseShield2.color = tempDefColor;
                break;
            case 3: tempDefColor.a = 1;
                defenseShield1.color = tempDefColor;
                defenseShield2.color = tempDefColor;
                defenseShield3.color = tempDefColor;
                break;
            case 4: tempDefColor.a = 1;
                defenseShield1.color = tempDefColor;
                defenseShield2.color = tempDefColor;
                defenseShield3.color = tempDefColor;
                defenseShield4.color = tempDefColor;
                break;
            default:
                break;
        }
        Debug.Log("current defense is now " + currentDefense);
    }
}
